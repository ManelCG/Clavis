#pragma once

#include <error/ClavisError.h>
#include <filesystem>

namespace Clavis::PasswordStoreElements {
    // These values are sort keys: SortElements orders elements by type first, so the declaration
    // order below IS the display order in the folderview (workspaces, folders, passwords, 2FA,
    // everything else). They are never persisted, so renumbering is safe.
    enum class PasswordStoreElementType {
        UNDEFINED = 0,

        WORKSPACE = 1,
        FOLDER = 2,
        GPG_FILE = 3,
        TWOFA_FILE = 4,
        FILE_WITHOUT_EXTENSION = 5,

        UNKNOWN = ~0,
    };

    std::string PasswordStoreElementTypeToString(const PasswordStoreElementType& type);



    class PasswordStoreElement {
    public:
        PasswordStoreElement(std::filesystem::path _p);

        // A workspace has no file behind it: it lives as an object in the workspaces database and
        // only borrows a path so that its name and the folder it appears in can be read back out
        // the same way they are for everything else.
        static PasswordStoreElement MakeWorkspace(const std::filesystem::path& virtualPath);

        // A workspace entry whose target is gone from disk. It is still classified by extension,
        // so a missing 2FA entry keeps sorting and rendering as one, but it is flagged so the UI
        // can show it as broken instead of pretending it can be opened.
        static PasswordStoreElement MakeMissing(const std::filesystem::path& target);

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
        bool IsWorkspace() const;
        bool IsMissing() const;
        bool IsHiddenFile() const;

        // True for anything Clavis can decrypt, i.e. both passwords and 2FA entries.
        bool IsEncryptedFile() const;

        std::string ToString() const;

        void SetDisplayName(const std::string& name);

    protected:

    private:
        // Bypasses the on-disk existence check the public constructor performs. Only the two
        // factories above use it, for the two kinds of element that have no file behind them.
        PasswordStoreElement(std::filesystem::path _p, PasswordStoreElementType _type);

        static PasswordStoreElementType ClassifyByExtension(const std::filesystem::path& p);

        std::filesystem::path path;
        PasswordStoreElementType type;
        std::string displayName;
        bool missing = false;
    };
}