#include <two_factor/MigrationImport.h>

#include <crypto/Base64.h>
#include <extensions/StringHelper.h>
#include <extensions/UriHelper.h>

// Google Authenticator's export format is a protobuf message. Rather than take a dependency on
// libprotobuf for one message, this decodes the wire format directly -- it only needs varints and
// length-delimited fields:
//
//     MigrationPayload { repeated OtpParameters otp_parameters = 1; ... }
//     OtpParameters {
//         bytes  secret    = 1;   // RAW bytes, not Base32
//         string name      = 2;   // may itself be "Issuer:account"
//         string issuer    = 3;
//         enum   algorithm = 4;   // 1=SHA1 2=SHA256 3=SHA512 4=MD5
//         enum   digits    = 5;   // 1=SIX 2=EIGHT
//         enum   type      = 6;   // 1=HOTP 2=TOTP
//         int64  counter   = 7;
//     }

namespace Clavis::TwoFactor {
    namespace {
        class WireReader {
        public:
            WireReader(const uint8_t* data, size_t size) : data(data), size(size), position(0) {}

            [[nodiscard]] bool IsAtEnd() const { return position >= size; }

            bool TryReadVarint(uint64_t& out) {
                out = 0;
                int shift = 0;

                while (position < size) {
                    const uint8_t byte = data[position++];
                    out |= static_cast<uint64_t>(byte & 0x7F) << shift;

                    if ((byte & 0x80) == 0)
                        return true;

                    shift += 7;
                    if (shift >= 64)
                        return false;
                }

                return false;
            }

            bool TryReadLengthDelimited(const uint8_t*& outData, size_t& outSize) {
                uint64_t length = 0;
                if (!TryReadVarint(length))
                    return false;

                if (length > size - position)
                    return false;

                outData = data + position;
                outSize = static_cast<size_t>(length);
                position += static_cast<size_t>(length);

                return true;
            }

            // Advances past a field whose contents we do not care about.
            bool TrySkip(uint32_t wireType) {
                switch (wireType) {
                    case 0: {
                        uint64_t ignored = 0;
                        return TryReadVarint(ignored);
                    }
                    case 1:
                        if (size - position < 8)
                            return false;
                        position += 8;
                        return true;
                    case 2: {
                        const uint8_t* ignoredData = nullptr;
                        size_t ignoredSize = 0;
                        return TryReadLengthDelimited(ignoredData, ignoredSize);
                    }
                    case 5:
                        if (size - position < 4)
                            return false;
                        position += 4;
                        return true;
                    default:
                        return false;
                }
            }

        private:
            const uint8_t* data;
            size_t size;
            size_t position;
        };

        bool TryParseOtpParameters(const uint8_t* data, size_t size,
                                   TwoFactorEntry& out, std::string& outUnsupportedName) {
            WireReader reader(data, size);

            std::vector<uint8_t> secret;
            std::string name;
            std::string issuer;
            uint64_t algorithmValue = 1;
            uint64_t digitsValue = 1;
            uint64_t typeValue = 2;
            uint64_t counterValue = 0;

            while (!reader.IsAtEnd()) {
                uint64_t tag = 0;
                if (!reader.TryReadVarint(tag))
                    return false;

                const auto fieldNumber = static_cast<uint32_t>(tag >> 3);
                const auto wireType = static_cast<uint32_t>(tag & 0x07);

                if (fieldNumber == 1 && wireType == 2) {
                    const uint8_t* fieldData = nullptr;
                    size_t fieldSize = 0;
                    if (!reader.TryReadLengthDelimited(fieldData, fieldSize))
                        return false;
                    secret.assign(fieldData, fieldData + fieldSize);
                } else if (fieldNumber == 2 && wireType == 2) {
                    const uint8_t* fieldData = nullptr;
                    size_t fieldSize = 0;
                    if (!reader.TryReadLengthDelimited(fieldData, fieldSize))
                        return false;
                    name.assign(reinterpret_cast<const char*>(fieldData), fieldSize);
                } else if (fieldNumber == 3 && wireType == 2) {
                    const uint8_t* fieldData = nullptr;
                    size_t fieldSize = 0;
                    if (!reader.TryReadLengthDelimited(fieldData, fieldSize))
                        return false;
                    issuer.assign(reinterpret_cast<const char*>(fieldData), fieldSize);
                } else if (fieldNumber == 4 && wireType == 0) {
                    if (!reader.TryReadVarint(algorithmValue))
                        return false;
                } else if (fieldNumber == 5 && wireType == 0) {
                    if (!reader.TryReadVarint(digitsValue))
                        return false;
                } else if (fieldNumber == 6 && wireType == 0) {
                    if (!reader.TryReadVarint(typeValue))
                        return false;
                } else if (fieldNumber == 7 && wireType == 0) {
                    if (!reader.TryReadVarint(counterValue))
                        return false;
                } else if (!reader.TrySkip(wireType))
                    return false;
            }

            if (secret.empty())
                return false;

            // The name is often "Issuer:account" even when the issuer field is also set.
            std::string account = name;
            if (const auto colon = name.find(':'); colon != std::string::npos) {
                if (issuer.empty())
                    issuer = StringHelper::Trim(name.substr(0, colon));
                account = StringHelper::Trim(name.substr(colon + 1));
            }

            const auto describe = issuer.empty() ? account : issuer + " - " + account;

            Crypto::HashAlgorithm algorithm;
            switch (algorithmValue) {
                case 0:
                case 1: algorithm = Crypto::HashAlgorithm::SHA1; break;
                case 2: algorithm = Crypto::HashAlgorithm::SHA256; break;
                case 3: algorithm = Crypto::HashAlgorithm::SHA512; break;
                default:
                    // MD5 (4) and anything newer: report it instead of importing a broken entry.
                    outUnsupportedName = describe;
                    return false;
            }

            TwoFactorEntry entry;
            entry.SetSecretBytes(secret);
            entry.SetIssuer(issuer);
            entry.SetAccount(account);
            entry.SetAlgorithm(algorithm);
            entry.SetDigits(digitsValue == 2 ? 8 : DEFAULT_DIGITS);
            entry.SetType(typeValue == 1 ? OtpType::HOTP : OtpType::TOTP);
            entry.SetCounter(counterValue);

            if (StringHelper::ToLower(issuer) == "steam") {
                entry.SetEncoder(OtpEncoder::STEAM);
                entry.SetDigits(STEAM_DIGITS);
            }

            out = entry;
            return true;
        }
    }

    bool TryParseMigrationUri(const std::string& uri, MigrationResult& out) {
        out.entries.clear();
        out.unsupportedAccounts.clear();

        const auto trimmed = StringHelper::Trim(uri);
        const auto scheme = std::string(OTPAUTH_MIGRATION_SCHEME);

        if (StringHelper::ToLower(trimmed).rfind(scheme, 0) != 0)
            return false;

        const auto question = trimmed.find('?');
        if (question == std::string::npos)
            return false;

        const auto query = UriHelper::ParseQuery(trimmed.substr(question + 1));

        std::string encodedData;
        if (!UriHelper::TryGetQueryValue(query, "data", encodedData) || encodedData.empty())
            return false;

        std::vector<uint8_t> payload;
        if (!Crypto::Base64::TryDecode(encodedData, payload) || payload.empty())
            return false;

        WireReader reader(payload.data(), payload.size());

        while (!reader.IsAtEnd()) {
            uint64_t tag = 0;
            if (!reader.TryReadVarint(tag))
                return false;

            const auto fieldNumber = static_cast<uint32_t>(tag >> 3);
            const auto wireType = static_cast<uint32_t>(tag & 0x07);

            if (fieldNumber == 1 && wireType == 2) {
                const uint8_t* fieldData = nullptr;
                size_t fieldSize = 0;
                if (!reader.TryReadLengthDelimited(fieldData, fieldSize))
                    return false;

                TwoFactorEntry entry;
                std::string unsupported;
                if (TryParseOtpParameters(fieldData, fieldSize, entry, unsupported))
                    out.entries.push_back(entry);
                else if (!unsupported.empty())
                    out.unsupportedAccounts.push_back(unsupported);
            } else if (!reader.TrySkip(wireType))
                return false;
        }

        return !out.entries.empty() || !out.unsupportedAccounts.empty();
    }
}
