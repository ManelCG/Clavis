#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace Clavis {
    class GPG {
    public:
        enum class KeyType {
            RSA_DSA                 = 1,
            DSA_ELGAMAL             = 2,
            ECC_25519               = 3,
            ECC_NIST_P384           = 4,
            ECC_BRAINPOOL_P256      = 5,
        };
        struct KeySizeRange {
            int min;
            int max;
            int def;
        };
        struct Key {
            std::string username;
            std::string keyname;
            std::string comment;

            KeyType type;
            int length;
        };

        static bool TryDecrypt(const std::filesystem::path& path, std::string& out);
        static bool TryDecrypt(const std::vector<uint8_t>& data, std::string& out);

        static bool TryEncrypt(const std::string& data, std::vector<uint8_t>& out);

        static std::vector<Key> GetAllKeys();
        static std::string KeyToString(const Key& key);

        static std::string KeyTypeToStringCode(KeyType type);
        static std::string KeyTypeToString(KeyType type);
        static KeyType StringCodeToKeyType(const std::string& str);
        static std::vector<KeyType> GetAllKeyTypes();
        static KeySizeRange GetKeySizeRange(const KeyType& type);

    protected:

    private:
        static std::string GetGPGID();

    };
}
