#pragma once

#include <filesystem>

#include <error/ClavisError.h>
#include <settings/Settings.h>
#include <password_store/PasswordStoreElement.h>

#include <password_store/Password.h>

namespace Clavis {
    class PasswordStore {
    public:
        PasswordStore() = default;
        static PasswordStore Initialize();

        bool IsAtRoot() const;

        bool GoUp();
        bool Chdir(const PasswordStoreElements::PasswordStoreElement& dir);

        bool DoesPasswordExist(const std::string& passwordName) const;
        bool TrySaveEncrypted(const std::string& passwordName, const std::vector<uint8_t>& encryptedPassword);

        int GetNumberOfPasswords() const;
        int GetNumberOfFolders() const;

        [[nodiscard]] std::filesystem::path GetRoot() const;
        [[nodiscard]] std::filesystem::path GetPath(bool relative = false) const;

        bool TryDecryptPassword(const PasswordStoreElements::PasswordStoreElement& elem, Password& password);

        [[nodiscard]] std::vector<PasswordStoreElements::PasswordStoreElement> GetElements() const;
        [[nodiscard]] std::vector<PasswordStoreElements::PasswordStoreElement> GetElements(std::string filter) const;

    protected:

    private:
        explicit PasswordStore(std::filesystem::path path);

        static std::vector<PasswordStoreElements::PasswordStoreElement> SortElements(const std::vector<PasswordStoreElements::PasswordStoreElement>& elements);

        std::filesystem::path root_path;
        std::filesystem::path store_path;

        bool isFilterCaseSensitive;
    };
}
