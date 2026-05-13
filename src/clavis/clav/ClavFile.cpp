#include <clav/ClavFile.h>

#include <error/ClavisError.h>
#include <extensions/GPGWrapper.h>
#include <language/Language.h>
#include <system/Extensions.h>

namespace Clavis::Clav {

    void ClavFile::WriteU32(std::vector<uint8_t>& buf, uint32_t v) {
        buf.push_back( v        & 0xFF);
        buf.push_back((v >>  8) & 0xFF);
        buf.push_back((v >> 16) & 0xFF);
        buf.push_back((v >> 24) & 0xFF);
    }

    void ClavFile::WriteBlob(std::vector<uint8_t>& buf, const std::vector<uint8_t>& data) {
        WriteU32(buf, static_cast<uint32_t>(data.size()));
        buf.insert(buf.end(), data.begin(), data.end());
    }

    void ClavFile::WriteString(std::vector<uint8_t>& buf, const std::string& s) {
        WriteU32(buf, static_cast<uint32_t>(s.size()));
        buf.insert(buf.end(), s.begin(), s.end());
    }

    std::vector<uint8_t> ClavFile::BuildPayload(const PasswordStore& store, const std::filesystem::path& searchRoot, const std::string& exportName) {
        std::vector<uint8_t> payload;
        const auto storeRoot = store.GetRoot();

        // Export name (first field — useful for import)
        WriteString(payload, exportName);

        // Public GPG key
        std::string gpgid;
        std::vector<uint8_t> pubKeyData;
        if (PasswordStore::TryGetGPGID(gpgid) && GPG::TryExportKey(gpgid, false, pubKeyData))
            WriteBlob(payload, pubKeyData);
        else
            WriteU32(payload, 0);

        // Collect .gpg files under searchRoot; paths are relative to the store root
        auto allFiles = System::ListContents(searchRoot, true, {".git"});
        std::vector<std::filesystem::path> gpgFiles;
        for (const auto& f : allFiles)
            if (f.extension() == ".gpg")
                gpgFiles.push_back(f);

        WriteU32(payload, static_cast<uint32_t>(gpgFiles.size()));
        for (const auto& f : gpgFiles) {
            WriteString(payload, std::filesystem::relative(f, storeRoot).string());
            std::vector<uint8_t> fileData;
            System::TryReadFile(f, fileData);
            WriteBlob(payload, fileData);
        }

        return payload;
    }

    static std::vector<uint8_t> EncryptAndFrame(
        const std::vector<uint8_t>& payload,
        EncryptionType encryption,
        const std::string& password)
    {
        std::vector<uint8_t> out;
        out.insert(out.end(), std::begin(ClavFile::MAGIC), std::end(ClavFile::MAGIC));
        out.push_back(ClavFile::VERSION);
        out.push_back(static_cast<uint8_t>(encryption));

        if (encryption == EncryptionType::None) {
            out.insert(out.end(), payload.begin(), payload.end());

        } else if (encryption == EncryptionType::Password) {
            std::vector<uint8_t> encrypted;
            if (!GPG::TryEncryptSymmetric(password, payload, encrypted))
                RaiseClavisError(_(ERROR_EXPORT_FAILED));
            out.insert(out.end(), encrypted.begin(), encrypted.end());

        } else {
            std::string payloadStr(reinterpret_cast<const char*>(payload.data()), payload.size());
            std::vector<uint8_t> encrypted;
            if (!GPG::TryEncrypt(payloadStr, encrypted))
                RaiseClavisError(_(ERROR_EXPORT_FAILED));
            out.insert(out.end(), encrypted.begin(), encrypted.end());
        }

        return out;
    }

    std::vector<uint8_t> ClavFile::Build(const PasswordStore& store, const std::string& exportName, EncryptionType encryption, const std::string& password) {
        return EncryptAndFrame(BuildPayload(store, store.GetRoot(), exportName), encryption, password);
    }

    std::vector<uint8_t> ClavFile::Build(const PasswordStore& store, const std::filesystem::path& subfolderPath, const std::string& exportName, EncryptionType encryption, const std::string& password) {
        return EncryptAndFrame(BuildPayload(store, subfolderPath, exportName), encryption, password);
    }

}
