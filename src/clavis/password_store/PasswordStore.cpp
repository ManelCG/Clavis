#include <algorithm>

#include <password_store/PasswordStore.h>
#include <system/Extensions.h>
#include <extensions/StringHelper.h>

namespace Clavis {
    PasswordStore::PasswordStore(std::filesystem::path path) {
        root_path = path;
        store_path = path;

        isFilterCaseSensitive = Settings::FILTER_CASE_SENSITIVE.GetValue();
    }

    PasswordStore PasswordStore::Initialize() {
        auto path = Settings::GetAsValue<std::string>(Settings::PASSWORD_STORE_PATH);
        if (!System::DirectoryExists(path))
            if (! System::mkdir_p(path))
                RaiseClavisError(_(ERROR_UNABLE_TO_CREATE_DIRECTORY, path));

        auto ps = PasswordStore(path);

        return ps;
    }

    std::vector<PasswordStoreElements::PasswordStoreElement> PasswordStore::GetElements() const {
        auto contents = System::ListContents(store_path);

        auto show_hidden_files = Settings::GetAsValue<bool>(Settings::SHOW_HIDDEN_FILES);

        std::vector<PasswordStoreElements::PasswordStoreElement> ret;

        for (const auto& file : contents) {
            auto elem =  PasswordStoreElements::PasswordStoreElement(file);

            if (!show_hidden_files && elem.IsHiddenFile())
                continue;

            ret.push_back(elem);
        }

        return SortElements(ret);
    }

    std::vector<PasswordStoreElements::PasswordStoreElement> PasswordStore::GetElements(std::string filter) const {
        auto elements = GetElements();
        std::vector<PasswordStoreElements::PasswordStoreElement> ret;

        if (!isFilterCaseSensitive)
            filter = StringHelper::ToLower(filter);


        for (const auto& elem : elements) {
            auto name = elem.GetName();
            if (!isFilterCaseSensitive)
                name = StringHelper::ToLower(name);

            if (name.find(filter) != std::string::npos)
                ret.push_back(elem);
        }

        return ret;
    }

    bool PasswordStore::TryDecryptPassword(const PasswordStoreElements::PasswordStoreElement &elem, Password& password) {
        if (!elem.IsGPGFile())
            RaiseClavisError(_(ERROR_NOT_A_PASSWORD, elem.GetName()));

        auto pw = Password::FromFile(elem.GetPath());

        if (!pw.TryDecrypt())
            return false;

        password = pw;
        return true;
    }

    std::filesystem::path PasswordStore::GetRoot() const {
        return root_path;
    }
    std::filesystem::path PasswordStore::GetPath(bool relative) const {
        if (!relative)
            return store_path;

        return std::filesystem::relative(store_path, root_path);
    }


    bool PasswordStore::IsAtRoot() const {
        return store_path ==  root_path;
    }

    bool PasswordStore::GoUp() {
        if (IsAtRoot())
            return false;

        store_path = store_path.parent_path();
        return true;
    }

    bool PasswordStore::Chdir(const PasswordStoreElements::PasswordStoreElement &dir) {
        auto path = dir.GetPath();
        if (!System::DirectoryExists(path))
            RaiseClavisError(_(ERROR_NOT_A_DIRECTORY, path.string()));

        store_path = path;
        return true;
    }

    bool PasswordStore::DoesPasswordExist(const std::string &passwordName) const {
        auto elements = GetElements();
        for (const auto& elem : elements) {
            if (elem.GetName() == passwordName)
                return true;
        }

        return false;
    }

    bool PasswordStore::TrySaveEncrypted(const std::string &passwordName, const std::vector<uint8_t> &encryptedPassword) {
        RaiseClavisError(_(ERROR_NOT_IMPLEMENTED));
    }

    int PasswordStore::GetNumberOfFolders() const {
        return System::GetNumberOfSubdirectories(GetRoot(), {".git"});
    }
    int PasswordStore::GetNumberOfPasswords() const {
        return System::GetNumberOfFiles(GetRoot(), ".gpg", {".git"});
    }




    std::vector<PasswordStoreElements::PasswordStoreElement> PasswordStore::SortElements(const std::vector<PasswordStoreElements::PasswordStoreElement> &elements) {
        std::vector<PasswordStoreElements::PasswordStoreElement> ret = elements;

        std::sort(ret.begin(), ret.end(), [](const auto& a, const auto& b) {
            if (a.GetType() != b.GetType())
                return static_cast<uint>(a.GetType()) < static_cast<uint>(b.GetType());
            return a.GetPath().filename().string() < b.GetPath().filename().string();
        });

        return ret;
    }

}