#pragma once

#include <error/ClavisError.h>
#include <filesystem>

namespace Clavis::PasswordStoreElements {
    // These values are sort keys: SortElements orders elements by type first, so the declaration
    // order below IS the display order in the folderview (folders, passwords, 2FA, everything
    // else). They are never persisted, so renumbering is safe.
    enum class PasswordStoreElementType {
        UNDEFINED = 0,

        FOLDER = 1,
        GPG_FILE = 2,
        TWOFA_FILE = 3,
        FILE_WITHOUT_EXTENSION = 4,

        UNKNOWN = ~0,
    };

    std::string PasswordStoreElementTypeToString(const PasswordStoreElementType& type);



    class PasswordStoreElement {
    public:
        PasswordStoreElement(std::filesystem::path _p);

        PasswordStoreElementType GetType() const;
        std::filesystem::path GetPath() const;

        // The on-disk name, extension included. This is the identity used for existence checks
        // and filesystem operations, so it must stay exact.
        std::string GetName() const;

        // The name as it should be shown to the user: the extension is dropped only when it is
        // one Clavis owns (.gpg, .2fa). Anything else is left intact, because a trailing ".com"
        // or ".net" is part of the entry's name rather than a file type.
        std::string GetLabel() const;

        bool IsGPGFile() const;
        bool IsTwoFactorFile() const;
        bool IsFolder() const;
        bool IsHiddenFile() const;

        // True for anything Clavis can decrypt, i.e. both passwords and 2FA entries.
        bool IsEncryptedFile() const;

        std::string ToString() const;

        void SetDisplayName(const std::string& name);

    protected:

    private:
        std::filesystem::path path;
        PasswordStoreElementType type;
        std::string displayName;
    };
}