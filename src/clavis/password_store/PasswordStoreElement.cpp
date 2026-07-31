#include <password_store/PasswordStoreElement.h>
#include <extensions/StringHelper.h>
#include <system/Extensions.h>
#include <two_factor/TwoFactorDefinitions.h>

#include "language/Language.h"

namespace Clavis::PasswordStoreElements {
    PasswordStoreElement::PasswordStoreElement(std::filesystem::path _p) {
        if ((!System::FileExists(_p) && (!System::DirectoryExists(_p))))
            RaiseClavisError(_(ERROR_NOT_A_DIRECTORY, _p.string()));

        path = _p;

        if (System::DirectoryExists(path))
            type = PasswordStoreElementType::FOLDER;
        else {
            // It is a file
            if (!path.has_parent_path())
                type = PasswordStoreElementType::FOLDER;
            else {
                auto ext =  path.extension().string();

                if (ext == ".gpg")
                    type = PasswordStoreElementType::GPG_FILE;
                else if (ext == TwoFactor::TWOFA_EXTENSION)
                    type = PasswordStoreElementType::TWOFA_FILE;
                else
                    type = PasswordStoreElementType::UNKNOWN;
            }
        }
    }

    std::string PasswordStoreElementTypeToString(const PasswordStoreElementType &type) {
        switch (type) {
            case PasswordStoreElementType::UNDEFINED:
                return "UDF";
            case PasswordStoreElementType::FOLDER:
                return "DIR";
            case PasswordStoreElementType::FILE_WITHOUT_EXTENSION:
                return "REG";
            case PasswordStoreElementType::GPG_FILE:
                return "GPG";
            case PasswordStoreElementType::TWOFA_FILE:
                return "2FA";
            default:
                return "UNK";
        }
    }


    std::string PasswordStoreElement::ToString() const {
        return PasswordStoreElementTypeToString(type) + " | " + path.filename().string();
    }

    PasswordStoreElementType PasswordStoreElement::GetType() const {
        return type;
    }

    std::filesystem::path PasswordStoreElement::GetPath() const {
        return path;
    }

    std::string PasswordStoreElement::GetName() const {
        if (!displayName.empty())
            return displayName;
        return GetPath().filename().string();
    }

    std::string PasswordStoreElement::GetLabel() const {
        const auto name = GetName();

        std::string knownExtension;
        if (type == PasswordStoreElementType::GPG_FILE)
            knownExtension = ".gpg";
        else if (type == PasswordStoreElementType::TWOFA_FILE)
            knownExtension = TwoFactor::TWOFA_EXTENSION;

        if (knownExtension.empty() || !StringHelper::EndsWith(name, knownExtension))
            return name;

        return name.substr(0, name.size() - knownExtension.size());
    }

    void PasswordStoreElement::SetDisplayName(const std::string& name) {
        displayName = name;
    }

    bool PasswordStoreElement::IsFolder() const {
        return type == PasswordStoreElementType::FOLDER;
    }

    bool PasswordStoreElement::IsGPGFile() const {
        return type == PasswordStoreElementType::GPG_FILE;
    }

    bool PasswordStoreElement::IsTwoFactorFile() const {
        return type == PasswordStoreElementType::TWOFA_FILE;
    }

    bool PasswordStoreElement::IsEncryptedFile() const {
        return IsGPGFile() || IsTwoFactorFile();
    }


    bool PasswordStoreElement::IsHiddenFile() const {
        auto name = GetName();
        if (name.size() == 0)
            return false;

        return name[0] == '.';
    }


}
