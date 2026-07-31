#include <two_factor/TwoFactorEntry.h>

#include <algorithm>

#include <crypto/Base32.h>
#include <extensions/StringHelper.h>
#include <extensions/UriHelper.h>
#include <system/Extensions.h>
#include <two_factor/OTP.h>

namespace Clavis::TwoFactor {
    namespace {
        // Metadata values are stored one per line, so embedded newlines are escaped rather than
        // written as continuation lines. That keeps Serialize/TryFromFileContents an exact
        // round trip even for multi-line notes.
        std::string EscapeValue(const std::string& value) {
            std::string out;
            for (const char c : value) {
                if (c == '\\')
                    out += "\\\\";
                else if (c == '\n')
                    out += "\\n";
                else if (c == '\r')
                    continue;
                else
                    out += c;
            }

            return out;
        }

        std::string UnescapeValue(const std::string& value) {
            std::string out;
            for (size_t i = 0; i < value.size(); i++) {
                if (value[i] != '\\' || i + 1 >= value.size()) {
                    out += value[i];
                    continue;
                }

                const char next = value[i + 1];
                if (next == 'n') {
                    out += '\n';
                    i++;
                } else if (next == '\\') {
                    out += '\\';
                    i++;
                } else {
                    out += value[i];
                }
            }

            return out;
        }

        bool IsValidMetadataKey(const std::string& key) {
            if (key.empty())
                return false;

            return std::all_of(key.begin(), key.end(), [](const char c) {
                return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                       (c >= '0' && c <= '9') || c == '_' || c == '-';
            });
        }

        std::string SanitizeForFilename(const std::string& s) {
            std::string out;
            for (const char c : s) {
                const auto byte = static_cast<unsigned char>(c);

                if (byte < 0x20 || c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' ||
                    c == '"' || c == '<' || c == '>' || c == '|')
                    out += '-';
                else
                    out += c;
            }

            return StringHelper::Trim(out);
        }
    }

    TwoFactorEntry::TwoFactorEntry()
        : type(OtpType::TOTP),
          encoder(OtpEncoder::DEFAULT),
          algorithm(Crypto::HashAlgorithm::SHA1),
          digits(DEFAULT_DIGITS),
          period(DEFAULT_PERIOD),
          counter(0) {
    }

    TwoFactorEntry::~TwoFactorEntry() {
        if (!secretBase32.empty())
            System::SecureZero(secretBase32.data(), secretBase32.size());

        if (!secretBytes.empty())
            System::SecureZero(secretBytes.data(), secretBytes.size());
    }

#pragma region PARSING
    bool TwoFactorEntry::TryFromUri(const std::string& uri, TwoFactorEntry& out) {
        const auto trimmed = StringHelper::Trim(uri);
        const auto scheme = std::string(OTPAUTH_SCHEME);

        if (StringHelper::ToLower(trimmed).rfind(scheme, 0) != 0)
            return false;

        const auto rest = trimmed.substr(scheme.size());

        const auto slash = rest.find('/');
        if (slash == std::string::npos)
            return false;

        const auto host = StringHelper::ToLower(rest.substr(0, slash));
        auto remainder = rest.substr(slash + 1);

        std::string rawLabel = remainder;
        std::string rawQuery;
        if (const auto question = remainder.find('?'); question != std::string::npos) {
            rawLabel = remainder.substr(0, question);
            rawQuery = remainder.substr(question + 1);
        }

        TwoFactorEntry entry;

        // A few exporters use otpauth://steam/... instead of the encoder parameter.
        if (host == "totp")
            entry.type = OtpType::TOTP;
        else if (host == "hotp")
            entry.type = OtpType::HOTP;
        else if (host == "steam") {
            entry.type = OtpType::TOTP;
            entry.encoder = OtpEncoder::STEAM;
        } else
            return false;

        std::string label;
        if (!UriHelper::TryPercentDecode(rawLabel, label))
            return false;

        // The label is "Issuer:account" or just "account". Providers commonly emit
        // "Issuer:%20account", so the leading space is stripped.
        if (const auto colon = label.find(':'); colon != std::string::npos) {
            entry.issuer = StringHelper::Trim(label.substr(0, colon));
            entry.account = StringHelper::Trim(label.substr(colon + 1));
        } else {
            entry.account = StringHelper::Trim(label);
        }

        const auto query = UriHelper::ParseQuery(rawQuery);

        std::string secret;
        if (!UriHelper::TryGetQueryValue(query, "secret", secret))
            return false;

        if (!entry.TrySetSecretBase32(secret))
            return false;

        // The issuer parameter wins over the label prefix -- that is the documented precedence,
        // and Google Authenticator writes both.
        if (std::string value; UriHelper::TryGetQueryValue(query, "issuer", value) && !value.empty())
            entry.issuer = StringHelper::Trim(value);

        if (std::string value; UriHelper::TryGetQueryValue(query, "algorithm", value)) {
            Crypto::HashAlgorithm parsed;
            if (!Crypto::TryParseHashAlgorithm(value, parsed))
                return false;   // MD5 and friends: better to refuse than to emit wrong codes.

            entry.algorithm = parsed;
        }

        bool hasExplicitDigits = false;
        if (std::string value; UriHelper::TryGetQueryValue(query, "digits", value)) {
            try {
                entry.digits = std::clamp(std::stoi(value), MIN_DIGITS, MAX_DIGITS);
                hasExplicitDigits = true;
            } catch (const std::exception&) {
                return false;
            }
        }

        if (std::string value; UriHelper::TryGetQueryValue(query, "period", value)) {
            try {
                entry.period = std::max(1, std::stoi(value));
            } catch (const std::exception&) {
                return false;
            }
        }

        if (std::string value; UriHelper::TryGetQueryValue(query, "counter", value)) {
            try {
                entry.counter = static_cast<uint64_t>(std::stoull(value));
            } catch (const std::exception&) {
                return false;
            }
        }

        if (std::string value; UriHelper::TryGetQueryValue(query, "encoder", value)) {
            if (StringHelper::ToLower(value) == "steam")
                entry.encoder = OtpEncoder::STEAM;
        }

        // Steam never issues a standard 6-digit token, so an issuer of "Steam" without an
        // explicit digit count is unambiguous.
        if (entry.encoder != OtpEncoder::STEAM && StringHelper::ToLower(entry.issuer) == "steam" &&
            (!hasExplicitDigits || entry.digits == STEAM_DIGITS))
            entry.encoder = OtpEncoder::STEAM;

        if (entry.encoder == OtpEncoder::STEAM) {
            entry.type = OtpType::TOTP;
            entry.algorithm = Crypto::HashAlgorithm::SHA1;
            entry.digits = STEAM_DIGITS;
            entry.period = STEAM_PERIOD;
        }

        out = entry;
        return true;
    }

    bool TwoFactorEntry::TryFromFileContents(const std::string& contents, TwoFactorEntry& out) {
        const auto lines = StringHelper::Split(contents, '\n');
        if (lines.empty())
            return false;

        TwoFactorEntry entry;
        if (!TryFromUri(lines[0], entry))
            return false;

        std::vector<MetadataField> fields;
        for (size_t i = 1; i < lines.size(); i++) {
            auto line = lines[i];
            if (!line.empty() && line.back() == '\r')
                line.pop_back();

            if (StringHelper::Trim(line).empty())
                continue;

            const auto colon = line.find(':');
            if (colon == std::string::npos)
                continue;

            const auto key = StringHelper::Trim(line.substr(0, colon));
            if (!IsValidMetadataKey(key))
                continue;

            auto value = line.substr(colon + 1);
            if (!value.empty() && value.front() == ' ')
                value.erase(value.begin());

            fields.push_back(MetadataField{ StringHelper::ToLower(key), UnescapeValue(value) });
        }

        entry.metadata = fields;

        out = entry;
        return true;
    }
#pragma endregion

#pragma region SERIALIZATION
    std::string TwoFactorEntry::BuildLabel() const {
        if (issuer.empty())
            return UriHelper::PercentEncode(account);

        // The colon separating issuer from account is structural, so the two components are
        // encoded individually and then joined.
        return UriHelper::PercentEncode(issuer) + ":" + UriHelper::PercentEncode(account);
    }

    std::string TwoFactorEntry::BuildUri() const {
        std::string uri = std::string(OTPAUTH_SCHEME);
        uri += type == OtpType::HOTP ? "hotp" : "totp";
        uri += "/";
        uri += BuildLabel();
        uri += "?secret=" + secretBase32;

        if (!issuer.empty())
            uri += "&issuer=" + UriHelper::PercentEncode(issuer);

        // Only non-default parameters are emitted, so the common case produces the short,
        // familiar URI that other authenticator apps display.
        if (algorithm != Crypto::HashAlgorithm::SHA1)
            uri += "&algorithm=" + Crypto::HashAlgorithmToString(algorithm);

        if (digits != DEFAULT_DIGITS)
            uri += "&digits=" + std::to_string(digits);

        if (type == OtpType::HOTP)
            uri += "&counter=" + std::to_string(counter);
        else if (period != DEFAULT_PERIOD)
            uri += "&period=" + std::to_string(period);

        if (encoder == OtpEncoder::STEAM)
            uri += "&encoder=steam";

        return uri;
    }

    std::string TwoFactorEntry::Serialize() const {
        std::string out = BuildUri();

        for (const auto& field : metadata) {
            if (field.key.empty())
                continue;

            out += "\n" + field.key + ": " + EscapeValue(field.value);
        }

        return out;
    }
#pragma endregion

#pragma region CODE GENERATION
    bool TwoFactorEntry::IsValid() const {
        return !secretBytes.empty() && digits >= MIN_DIGITS && digits <= MAX_DIGITS && period >= 1;
    }

    bool TwoFactorEntry::TryGenerateCode(std::string& outCode) const {
        return TryGenerateCode(OTP::GetUnixTime(), outCode);
    }

    bool TwoFactorEntry::TryGenerateCode(int64_t unixTime, std::string& outCode) const {
        if (!IsValid())
            return false;

        if (encoder == OtpEncoder::STEAM)
            outCode = OTP::GenerateSteam(secretBytes, unixTime, period);
        else if (type == OtpType::HOTP)
            outCode = OTP::GenerateHOTP(algorithm, secretBytes, counter, digits);
        else
            outCode = OTP::GenerateTOTP(algorithm, secretBytes, unixTime, period, digits);

        return true;
    }

    int TwoFactorEntry::GetSecondsRemaining() const {
        if (type == OtpType::HOTP)
            return 0;

        return OTP::GetSecondsRemaining(OTP::GetUnixTime(), period);
    }

    uint64_t TwoFactorEntry::GetCurrentTimeCounter() const {
        if (type == OtpType::HOTP)
            return counter;

        return OTP::GetTimeCounter(OTP::GetUnixTime(), period);
    }

    void TwoFactorEntry::AdvanceCounter() {
        if (type != OtpType::HOTP)
            return;

        counter++;
    }
#pragma endregion

#pragma region ACCESSORS
    std::string TwoFactorEntry::GetSuggestedName() const {
        const auto cleanIssuer = SanitizeForFilename(issuer);
        const auto cleanAccount = SanitizeForFilename(account);

        if (cleanIssuer.empty() && cleanAccount.empty())
            return "";

        if (cleanIssuer.empty())
            return cleanAccount;

        if (cleanAccount.empty())
            return cleanIssuer;

        return cleanIssuer + " - " + cleanAccount;
    }

    OtpType TwoFactorEntry::GetType() const { return type; }
    OtpEncoder TwoFactorEntry::GetEncoder() const { return encoder; }
    Crypto::HashAlgorithm TwoFactorEntry::GetAlgorithm() const { return algorithm; }
    const std::string& TwoFactorEntry::GetIssuer() const { return issuer; }
    const std::string& TwoFactorEntry::GetAccount() const { return account; }
    const std::string& TwoFactorEntry::GetSecretBase32() const { return secretBase32; }
    int TwoFactorEntry::GetDigits() const { return digits; }
    int TwoFactorEntry::GetPeriod() const { return period; }
    uint64_t TwoFactorEntry::GetCounter() const { return counter; }
    const std::vector<MetadataField>& TwoFactorEntry::GetMetadata() const { return metadata; }

    std::vector<std::string> TwoFactorEntry::GetRecoveryCodes() const {
        std::vector<std::string> codes;
        for (const auto& field : metadata) {
            if (field.key == METADATA_KEY_RECOVERY && !field.value.empty())
                codes.push_back(field.value);
        }

        return codes;
    }

    std::string TwoFactorEntry::GetNotes() const {
        for (const auto& field : metadata) {
            if (field.key == METADATA_KEY_NOTES)
                return field.value;
        }

        return "";
    }

    void TwoFactorEntry::SetType(OtpType value) { type = value; }
    void TwoFactorEntry::SetEncoder(OtpEncoder value) { encoder = value; }
    void TwoFactorEntry::SetAlgorithm(Crypto::HashAlgorithm value) { algorithm = value; }
    void TwoFactorEntry::SetIssuer(const std::string& value) { issuer = StringHelper::Trim(value); }
    void TwoFactorEntry::SetAccount(const std::string& value) { account = StringHelper::Trim(value); }
    void TwoFactorEntry::SetDigits(int value) { digits = std::clamp(value, MIN_DIGITS, MAX_DIGITS); }
    void TwoFactorEntry::SetPeriod(int value) { period = std::max(1, value); }
    void TwoFactorEntry::SetCounter(uint64_t value) { counter = value; }
    void TwoFactorEntry::SetMetadata(const std::vector<MetadataField>& value) { metadata = value; }

    bool TwoFactorEntry::TrySetSecretBase32(const std::string& value) {
        std::vector<uint8_t> decoded;
        if (!Crypto::Base32::TryDecode(value, decoded) || decoded.empty())
            return false;

        if (!secretBase32.empty())
            System::SecureZero(secretBase32.data(), secretBase32.size());
        if (!secretBytes.empty())
            System::SecureZero(secretBytes.data(), secretBytes.size());

        // Normalised to the canonical unpadded uppercase form so the URI we emit is stable
        // regardless of how the provider formatted the secret.
        secretBytes = decoded;
        secretBase32 = Crypto::Base32::Encode(decoded, false);

        return true;
    }

    void TwoFactorEntry::SetSecretBytes(const std::vector<uint8_t>& value) {
        if (!secretBase32.empty())
            System::SecureZero(secretBase32.data(), secretBase32.size());
        if (!secretBytes.empty())
            System::SecureZero(secretBytes.data(), secretBytes.size());

        secretBytes = value;
        secretBase32 = Crypto::Base32::Encode(value, false);
    }

    void TwoFactorEntry::SetRecoveryCodes(const std::vector<std::string>& codes) {
        std::vector<MetadataField> updated;

        // Recovery codes are rewritten as a block while every other field keeps its position.
        bool inserted = false;
        for (const auto& field : metadata) {
            if (field.key != METADATA_KEY_RECOVERY) {
                updated.push_back(field);
                continue;
            }

            if (inserted)
                continue;

            for (const auto& code : codes)
                updated.push_back(MetadataField{ METADATA_KEY_RECOVERY, code });
            inserted = true;
        }

        if (!inserted) {
            for (const auto& code : codes)
                updated.push_back(MetadataField{ METADATA_KEY_RECOVERY, code });
        }

        metadata = updated;
    }

    void TwoFactorEntry::SetNotes(const std::string& notes) {
        std::vector<MetadataField> updated;

        bool replaced = false;
        for (const auto& field : metadata) {
            if (field.key != METADATA_KEY_NOTES) {
                updated.push_back(field);
                continue;
            }

            if (replaced)
                continue;

            if (!notes.empty())
                updated.push_back(MetadataField{ METADATA_KEY_NOTES, notes });
            replaced = true;
        }

        if (!replaced && !notes.empty())
            updated.push_back(MetadataField{ METADATA_KEY_NOTES, notes });

        metadata = updated;
    }
#pragma endregion
}
