#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <password_store/PasswordStore.h>

namespace Clavis::Clav {

    enum class EncryptionType : uint8_t {
        None     = 0,
        Password = 1,
        GPGKey   = 2,
    };

    struct ClavEntry {
        std::string          relPath;
        std::vector<uint8_t> data;
    };

    struct ParsedClavFile {
        std::string            name;
        std::vector<uint8_t>   publicKeyData;
        std::vector<ClavEntry> entries;
        EncryptionType         encryption;
    };

    enum class ClavReadResult {
        Ok,
        NotAClavFile,
        UnsupportedVersion,
        DecryptionFailed,
        TruncatedData,
    };

    class ClavFile {
    public:
        static constexpr char    MAGIC[4] = {'C', 'L', 'A', 'V'};
        static constexpr uint8_t VERSION  = 1;

        // Export the whole store
        static std::vector<uint8_t> Build(
            const PasswordStore&  store,
            const std::string&    exportName,
            EncryptionType        encryption,
            const std::string&    password = ""
        );

        // Export only files under subfolderPath (paths in the payload remain store-root-relative)
        static std::vector<uint8_t> Build(
            const PasswordStore&           store,
            const std::filesystem::path&   subfolderPath,
            const std::string&             exportName,
            EncryptionType                 encryption,
            const std::string&             password = ""
        );

        static ClavReadResult TryCheckFormat(const std::vector<uint8_t>& fileData, EncryptionType& outEncryption);
        static ClavReadResult TryRead(const std::vector<uint8_t>& fileData, ParsedClavFile& out, const std::string& password = "");
        static bool           Unpack(const ParsedClavFile& file, const std::filesystem::path& targetDir);
        static bool           CheckPublicKeyMatchesStore(const std::vector<uint8_t>& keyData);

    private:
        static std::vector<uint8_t> BuildPayload(
            const PasswordStore&         store,
            const std::filesystem::path& searchRoot,
            const std::string&           exportName
        );

        static void WriteU32   (std::vector<uint8_t>& buf, uint32_t v);
        static void WriteBlob  (std::vector<uint8_t>& buf, const std::vector<uint8_t>& data);
        static void WriteString(std::vector<uint8_t>& buf, const std::string& s);

        static bool ReadU32   (const std::vector<uint8_t>& buf, size_t& offset, uint32_t& out);
        static bool ReadBlob  (const std::vector<uint8_t>& buf, size_t& offset, std::vector<uint8_t>& out);
        static bool ReadString(const std::vector<uint8_t>& buf, size_t& offset, std::string& out);
        static ClavReadResult ParsePayload(const std::vector<uint8_t>& payload, ParsedClavFile& out);
    };

}
