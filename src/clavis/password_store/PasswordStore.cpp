#include <algorithm>
#include <set>

#include <password_store/PasswordStore.h>
#include <system/Extensions.h>
#include <extensions/StringHelper.h>

#include <cstdint>

#ifdef __WINDOWS__
#define uint uint32_t
#endif

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

    std::vector<PasswordStoreElements::PasswordStoreElement> PasswordStore::GetElementsRecursive(std::string filter) const {
        auto contents = System::ListContents(store_path, true, {".git"});
        auto show_hidden_files = Settings::GetAsValue<bool>(Settings::SHOW_HIDDEN_FILES);

        if (!isFilterCaseSensitive)
            filter = StringHelper::ToLower(filter);

        struct MatchedItem {
            PasswordStoreElements::PasswordStoreElement elem;
            std::filesystem::path relPath;
            std::string matchKey;
        };

        std::vector<MatchedItem> matches;

        for (const auto& filePath : contents) {
            auto elem = PasswordStoreElements::PasswordStoreElement(filePath);

            if (!show_hidden_files && elem.IsHiddenFile())
                continue;

            auto relPath = std::filesystem::relative(filePath, store_path);
            auto matchKey = relPath.string();
            if (!isFilterCaseSensitive)
                matchKey = StringHelper::ToLower(matchKey);

            if (matchKey.find(filter) == std::string::npos)
                continue;

            elem.SetDisplayName(relPath.string());
            matches.push_back({elem, relPath, matchKey});
        }

        // Build a lookup set of all matched relative-path keys for ancestor checks.
        std::set<std::string> matchKeySet;
        for (const auto& m : matches)
            matchKeySet.insert(m.matchKey);

        std::vector<PasswordStoreElements::PasswordStoreElement> ret;

        for (const auto& m : matches) {
            bool suppressed = false;

            // Walk up the ancestor chain. If any ancestor is also in the result set,
            // suppress this entry — unless its own filename independently matches.
            auto ancestor = m.relPath.parent_path();
            while (!ancestor.empty()) {
                auto ancestorKey = ancestor.string();
                if (!isFilterCaseSensitive)
                    ancestorKey = StringHelper::ToLower(ancestorKey);

                if (matchKeySet.count(ancestorKey) > 0) {
                    auto ownName = m.relPath.filename().string();
                    if (!isFilterCaseSensitive)
                        ownName = StringHelper::ToLower(ownName);

                    if (ownName.find(filter) == std::string::npos)
                        suppressed = true;

                    break;
                }

                ancestor = ancestor.parent_path();
            }

            if (!suppressed)
                ret.push_back(m.elem);
        }

        return SortElements(ret);
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

    bool PasswordStore::TryDecryptElement(const PasswordStoreElements::PasswordStoreElement &elem, Password& password) {
        if (!elem.IsEncryptedFile())
            RaiseClavisError(_(ERROR_NOT_A_PASSWORD, elem.GetName()));

        auto pw = Password::FromFile(elem.GetPath());

        if (!pw.TryDecrypt())
            return false;

        password = pw;
        return true;
    }

    bool PasswordStore::TryDecryptTwoFactor(const PasswordStoreElements::PasswordStoreElement &elem,
                                            TwoFactor::TwoFactorEntry& entry) {
        if (!elem.IsTwoFactorFile())
            RaiseClavisError(_(ERROR_NOT_A_TWO_FACTOR, elem.GetName()));

        Password pw;
        if (!TryDecryptElement(elem, pw))
            return false;

        if (!TwoFactor::TwoFactorEntry::TryFromFileContents(pw.GetPassword(), entry))
            RaiseClavisError(_(ERROR_INVALID_TWO_FACTOR_FILE, elem.GetName()));

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

    bool PasswordStore::IsValidPasswordStore(const std::filesystem::path &p) {
        if (!System::DirectoryExists(p))
            return false;

        for (const auto& c : System::ListContents(p, false))
            if (System::FileExists(c) && c.filename() == ".gpg-id")
                return true;

        return false;
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

    int PasswordStore::GetNumberOfTwoFactorEntries() const {
        return System::GetNumberOfFiles(GetRoot(), TwoFactor::TWOFA_EXTENSION, {".git"});
    }

    std::string PasswordStore::GetGPGID() {
        std::string ret;
        if (!TryGetGPGID(ret))
            RaiseClavisError(_(ERROR_GPG_ID_FILE_NOT_FOUND, System::GetGPGIDPath()));

        return ret;
    }

    bool PasswordStore::TryGetGPGID(const std::filesystem::path &directory, std::string &outgpgid) {
        return __TryGetGPGID(directory / ".gpg-id", outgpgid);
    }


    bool PasswordStore::TryGetGPGID(std::string &outgpgid) {
        return __TryGetGPGID(System::GetGPGIDPath(), outgpgid);
    }

    bool PasswordStore::__TryGetGPGID(const std::filesystem::path &file, std::string &out) {
        if (!System::FileExists(file))
            return false;

        std::string id;
        if (!System::TryReadFile(file, id))
            RaiseClavisError(_(ERROR_CANNOT_READ_GPGID_FILE, file.string()));

        id.erase(id.find_last_not_of(" \r\n\t") + 1);
        out = id;

        return true;
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