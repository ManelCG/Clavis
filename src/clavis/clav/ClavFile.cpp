#include <clav/ClavFile.h>

#include <algorithm>

#include <error/ClavisError.h>
#include <extensions/GPGWrapper.h>
#include <language/Language.h>
#include <system/Extensions.h>
#include <password_store/PasswordStore.h>

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

    bool ClavFile::ReadU32(const std::vector<uint8_t>& buf, size_t& offset, uint32_t& out) {
        if (offset + 4 > buf.size()) return false;
        out = static_cast<uint32_t>(buf[offset])
            | static_cast<uint32_t>(buf[offset + 1]) << 8
            | static_cast<uint32_t>(buf[offset + 2]) << 16
            | static_cast<uint32_t>(buf[offset + 3]) << 24;
        offset += 4;
        return true;
    }

    bool ClavFile::ReadBlob(const std::vector<uint8_t>& buf, size_t& offset, std::vector<uint8_t>& out) {
        uint32_t len;
        if (!ReadU32(buf, offset, len)) return false;
        if (offset + len > buf.size()) return false;
        out.assign(buf.begin() + offset, buf.begin() + offset + len);
        offset += len;
        return true;
    }

    bool ClavFile::ReadString(const std::vector<uint8_t>& buf, size_t& offset, std::string& out) {
        uint32_t len;
        if (!ReadU32(buf, offset, len)) return false;
        if (offset + len > buf.size()) return false;
        out.assign(reinterpret_cast<const char*>(buf.data() + offset), len);
        offset += len;
        return true;
    }

    ClavReadResult ClavFile::ParsePayload(const std::vector<uint8_t>& payload, ParsedClavFile& out) {
        size_t offset = 0;
        if (!ReadString(payload, offset, out.name))        return ClavReadResult::TruncatedData;
        if (!ReadBlob  (payload, offset, out.publicKeyData)) return ClavReadResult::TruncatedData;

        uint32_t fileCount;
        if (!ReadU32(payload, offset, fileCount)) return ClavReadResult::TruncatedData;

        out.entries.resize(fileCount);
        for (uint32_t i = 0; i < fileCount; ++i) {
            if (!ReadString(payload, offset, out.entries[i].relPath)) return ClavReadResult::TruncatedData;
            if (!ReadBlob  (payload, offset, out.entries[i].data))    return ClavReadResult::TruncatedData;
        }
        return ClavReadResult::Ok;
    }

    ClavReadResult ClavFile::TryCheckFormat(const std::vector<uint8_t>& fileData, EncryptionType& outEncryption) {
        if (fileData.size() < 6) return ClavReadResult::NotAClavFile;
        if (fileData[0] != MAGIC[0] || fileData[1] != MAGIC[1] ||
            fileData[2] != MAGIC[2] || fileData[3] != MAGIC[3])
            return ClavReadResult::NotAClavFile;
        if (fileData[4] != VERSION) return ClavReadResult::UnsupportedVersion;
        outEncryption = static_cast<EncryptionType>(fileData[5]);
        return ClavReadResult::Ok;
    }

    ClavReadResult ClavFile::TryRead(const std::vector<uint8_t>& fileData, ParsedClavFile& out, const std::string& password) {
        EncryptionType enc;
        ClavReadResult result = TryCheckFormat(fileData, enc);
        if (result != ClavReadResult::Ok) return result;

        out.encryption = enc;
        std::vector<uint8_t> encData(fileData.begin() + 6, fileData.end());
        std::vector<uint8_t> payload;

        if (enc == EncryptionType::None) {
            payload = std::move(encData);
        } else if (enc == EncryptionType::Password) {
            if (!GPG::TryDecryptSymmetric(password, encData, payload))
                return ClavReadResult::DecryptionFailed;
        } else {
            std::string plainStr;
            if (!GPG::TryDecrypt(encData, plainStr))
                return ClavReadResult::DecryptionFailed;
            payload.assign(plainStr.begin(), plainStr.end());
        }

        return ParsePayload(payload, out);
    }

    bool ClavFile::CheckPublicKeyMatchesStore(const std::vector<uint8_t>& keyData) {
        if (keyData.empty()) return false;

        std::string storeGpgId;
        if (!PasswordStore::TryGetGPGID(storeGpgId)) return false;

        std::string storeFp;
        if (!GPG::TryGetKeyFingerprint(storeGpgId, storeFp)) return false;

        std::string importedFp;
        if (!GPG::TryImportKey(keyData, importedFp)) return false;

        auto toUpper = [](std::string s) {
            for (char& c : s) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
            return s;
        };
        return toUpper(importedFp) == toUpper(storeFp);
    }

    bool ClavFile::Unpack(const ParsedClavFile& file, const std::filesystem::path& targetDir) {
        for (const auto& entry : file.entries) {
            auto destPath = targetDir / entry.relPath;
            System::mkdir_p(destPath.parent_path());
            if (!System::TryWriteFile(destPath, entry.data))
                return false;
        }
        return true;
    }

    std::vector<uint8_t> ClavFile::BuildPayload(const PasswordStore& store, const std::filesystem::path& searchRoot, const std::string& exportName) {
        std::vector<uint8_t> payload;

        // Export name (first field — useful for import)
        WriteString(payload, exportName);

        // Public GPG key
        std::string gpgid;
        std::vector<uint8_t> pubKeyData;
        if (PasswordStore::TryGetGPGID(gpgid) && GPG::TryExportKey(gpgid, false, pubKeyData))
            WriteBlob(payload, pubKeyData);
        else
            WriteU32(payload, 0);

        // Collect .gpg files under searchRoot; paths are relative to searchRoot
        auto allFiles = System::ListContents(searchRoot, true, {".git"});
        std::vector<std::filesystem::path> gpgFiles;
        for (const auto& f : allFiles)
            if (f.extension() == ".gpg")
                gpgFiles.push_back(f);

        WriteU32(payload, static_cast<uint32_t>(gpgFiles.size()));
        for (const auto& f : gpgFiles) {
            WriteString(payload, std::filesystem::relative(f, searchRoot).string());
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
