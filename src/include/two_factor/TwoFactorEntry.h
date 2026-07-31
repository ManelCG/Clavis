#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <crypto/Hash.h>
#include <two_factor/TwoFactorDefinitions.h>

namespace Clavis::TwoFactor {
    struct MetadataField {
        std::string key;
        std::string value;
    };

    // A single second-factor credential, stored on disk as a GPG-encrypted `.2fa` file:
    //
    //     otpauth://totp/GitHub:me@example.com?secret=JBSWY3DPEHPK3PXP&issuer=GitHub
    //     recovery: 1a2b-3c4d
    //     recovery: 5e6f-7g8h
    //     notes: work account
    //
    // Line 1 is the canonical URI and the single source of truth for code generation. Every
    // following line is a "key: value" metadata field; unknown keys are preserved verbatim so a
    // future Clavis (or a hand-edited file) never loses data on a round trip.
    class TwoFactorEntry {
    public:
        TwoFactorEntry();
        ~TwoFactorEntry();

        static bool TryFromUri(const std::string& uri, TwoFactorEntry& out);
        static bool TryFromFileContents(const std::string& contents, TwoFactorEntry& out);

        [[nodiscard]] std::string BuildUri() const;
        [[nodiscard]] std::string Serialize() const;

        [[nodiscard]] bool IsValid() const;

        // Uses the current wall clock for TOTP/Steam and the stored counter for HOTP.
        [[nodiscard]] bool TryGenerateCode(std::string& outCode) const;
        [[nodiscard]] bool TryGenerateCode(int64_t unixTime, std::string& outCode) const;

        // Time-based entries only. Returns 0 for HOTP, which does not expire.
        [[nodiscard]] int GetSecondsRemaining() const;
        [[nodiscard]] uint64_t GetCurrentTimeCounter() const;

        void AdvanceCounter();

        // "Issuer - account", sanitised so it can be used as a filename. Never includes the
        // .2fa extension.
        [[nodiscard]] std::string GetSuggestedName() const;

        [[nodiscard]] OtpType GetType() const;
        [[nodiscard]] OtpEncoder GetEncoder() const;
        [[nodiscard]] Crypto::HashAlgorithm GetAlgorithm() const;
        [[nodiscard]] const std::string& GetIssuer() const;
        [[nodiscard]] const std::string& GetAccount() const;
        [[nodiscard]] const std::string& GetSecretBase32() const;
        [[nodiscard]] int GetDigits() const;
        [[nodiscard]] int GetPeriod() const;
        [[nodiscard]] uint64_t GetCounter() const;

        [[nodiscard]] const std::vector<MetadataField>& GetMetadata() const;
        [[nodiscard]] std::vector<std::string> GetRecoveryCodes() const;
        [[nodiscard]] std::string GetNotes() const;

        void SetType(OtpType value);
        void SetEncoder(OtpEncoder value);
        void SetAlgorithm(Crypto::HashAlgorithm value);
        void SetIssuer(const std::string& value);
        void SetAccount(const std::string& value);
        void SetDigits(int value);
        void SetPeriod(int value);
        void SetCounter(uint64_t value);

        // Returns false and leaves the entry untouched when the secret is not valid Base32.
        bool TrySetSecretBase32(const std::string& value);
        void SetSecretBytes(const std::vector<uint8_t>& value);

        void SetMetadata(const std::vector<MetadataField>& value);
        void SetRecoveryCodes(const std::vector<std::string>& codes);
        void SetNotes(const std::string& notes);

    protected:

    private:
        [[nodiscard]] std::string BuildLabel() const;

        OtpType type;
        OtpEncoder encoder;
        Crypto::HashAlgorithm algorithm;

        std::string issuer;
        std::string account;

        std::string secretBase32;
        std::vector<uint8_t> secretBytes;

        int digits;
        int period;
        uint64_t counter;

        std::vector<MetadataField> metadata;
    };
}
