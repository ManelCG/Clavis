#include <password_store/PasswordStoreElement.h>
#include <system/Extensions.h>

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
        return GetPath().filename().string();
    }

    bool PasswordStoreElement::IsFolder() const {
        return type == PasswordStoreElementType::FOLDER;
    }

    bool PasswordStoreElement::IsGPGFile() const {
        return type == PasswordStoreElementType::GPG_FILE;
    }


    bool PasswordStoreElement::IsHiddenFile() const {
        auto name = GetName();
        if (name.size() == 0)
            return false;

        return name[0] == '.';
    }


}
