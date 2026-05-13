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

    private:
        static std::vector<uint8_t> BuildPayload(
            const PasswordStore&         store,
            const std::filesystem::path& searchRoot,
            const std::string&           exportName
        );

        static void WriteU32   (std::vector<uint8_t>& buf, uint32_t v);
        static void WriteBlob  (std::vector<uint8_t>& buf, const std::vector<uint8_t>& data);
        static void WriteString(std::vector<uint8_t>& buf, const std::string& s);
    };

}
