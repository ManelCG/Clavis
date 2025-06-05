#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace Clavis {
    class GPG {
    public:
        struct KeySizeRange {
            int min;
            int max;
            int def;
        };
        enum class KeyType {
            RSA_DSA                 = 1001,
            DSA_ELGAMAL             = 1002,
            ECC_25519               = 2001,
            ECC_NIST_P256           = 3020,
            ECC_NIST_P384           = 3030,
            ECC_NIST_P521           = 3050,
            ECC_BRAINPOOL_P256      = 4020,
        };
        struct Key {
            std::string username;
            std::string keyname;
            std::string comment;

            std::string fingerprint;

            std::string password;
            KeyType type;
            int length;
        };

        static bool TryDecrypt(const std::filesystem::path& path, std::string& out);
        static bool TryDecrypt(const std::vector<uint8_t>& data, std::string& out);

        static bool TryEncrypt(const std::string& data, std::vector<uint8_t>& out);

        static bool TryCreateKey(const Key& data, std::string& outFingerprint);
        static bool TryImportKey(const std::vector<uint8_t>& data, std::string& outFingerprint);
        static bool TryExportKey(const std::string& gpgid, bool exportPrivate, std::vector<uint8_t>& out);

#ifdef __LINUX__
        static bool TryChangeKeyTrust(const std::string& fingerprint, int trustlevel);
#endif

        static std::vector<Key> GetAllKeys();
        static std::string KeyToString(const Key& key, bool escapeChars = false);
        static std::string KeyToStringFull(const Key& key);

        static std::string KeyTypeToStringCode(KeyType type);
        static std::string KeyTypeToString(KeyType type);
        static KeyType StringCodeToKeyType(const std::string& str);
        static std::vector<KeyType> GetAllKeyTypes();
        static KeySizeRange GetKeySizeRange(const KeyType& type);

    protected:

    private:
        static std::string GetGPGID();
        static std::string GetKeyParams(Key data);
    };
}
