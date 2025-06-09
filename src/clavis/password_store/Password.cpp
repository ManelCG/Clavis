#include <password_store/Password.h>

#include <error/ClavisError.h>

#include <extensions/GPGWrapper.h>
#include <system/Extensions.h>

#include "language/Language.h"

namespace Clavis {
    Password::Password() {
        isOnDisk = false;
        isDecrypted = false;
    }

    Password Password::FromFile(std::filesystem::path path) {
        if (! System::FileExists(path))
            RaiseClavisError(_(ERROR_NOT_A_FILE, path.string()));

        auto p = Password();

        p.path = path;
        p.isOnDisk = true;

        return p;
    }

    Password Password::FromPassword(const std::string& password) {
        auto p = Password();

        p.password = password;
        p.isDecrypted = true;

        return p;
    }

    bool Password::IsOnDisk() const {
        return isOnDisk;
    }
    bool Password::IsDecrypted() const {
        return isDecrypted;
    }

    const std::string& Password::GetPassword() const {
        if (!isDecrypted)
            RaiseClavisError(_(ERROR_PASSWORD_NOT_DECRYPTED));

        return password;
    }

    std::filesystem::path Password::GetPath() const {
        if (!isOnDisk)
            RaiseClavisError(_(ERROR_PASSWORD_NOT_ON_DISK));

        return path;
    }

    bool Password::TryDecrypt() {
        if (!isOnDisk)
            RaiseClavisError(_(ERROR_PASSWORD_NOT_ON_DISK));

        if (!GPG::TryDecrypt(path, password))
            return false;

        isDecrypted = true;
        password.erase(password.find_last_not_of(" \r\n\t") + 1);
        return true;
    }

    bool Password::TrySaveEncrypted(const std::filesystem::path &dest) {
        if (!isDecrypted)
            RaiseClavisError(_(ERROR_PASSWORD_NOT_DECRYPTED));

        std::vector<uint8_t> data;
        if (!GPG::TryEncrypt(password, data))
            return false;

        if (!System::TryWriteFile(dest, data))
            return false;

        isOnDisk = true;
        path = dest;

        return true;
    }

    // Try to really clear the pw from memory
    Password::~Password() {
        if (isDecrypted) {
            for (size_t i = 0; i < password.size(); i++) {
                password[i] = '\0';
            }

            password.clear();
            password = "";
        }
    }



}
