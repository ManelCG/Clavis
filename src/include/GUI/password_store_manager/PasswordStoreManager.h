#pragma once

#include <gtkmm.h>

#include <filesystem>
#include <optional>

#include <password_store/PasswordStore.h>
#include <password_store/Workspaces.h>

#include <GUI/password_store_manager/components/PasswordStoreFolderview.h>
#include <GUI/password_store_manager/components/PasswordStoreManagerTools.h>
#include <GUI/password_store_manager/components/PasswordStoreOutputDisplay.h>


namespace Clavis::GUI {
    class PasswordStoreManager : public Gtk::Box {
    public:
        PasswordStoreManager();

        PasswordStore GetPasswordStore() const;

        // By reference: workflows mutate the database in place and then Save() it, so handing out
        // a copy would silently discard their changes.
        Workspaces::WorkspaceDB& GetWorkspaceDB();
        void ReloadWorkspaces();

        // The workspace currently being browsed, as {store-relative directory, name}. Empty when
        // browsing the real tree.
        [[nodiscard]] bool IsInWorkspace() const;
        [[nodiscard]] std::filesystem::path GetActiveWorkspaceDir() const;
        [[nodiscard]] std::string GetActiveWorkspaceName() const;

        // Leaves the workspace being browsed, if any. Used after an edit renames or moves it, so
        // the view never points at a workspace that no longer exists under that name.
        void LeaveWorkspace();

        void Initialize();
        void Refresh();

        void PerformGitAction(GitManagerToolbar::Action action);

        // Re-renders the 2FA section, e.g. after an HOTP counter advance rewrote the entry.
        void DisplayTwoFactor(const TwoFactor::TwoFactorEntry& entry, const std::filesystem::path& path);

        // Clears anything decrypted on screen and flushes gpg-agent's key cache.
        void LockVault();


    protected:

    private:
        void GoUp();
        void Chdir(const PasswordStoreElements::PasswordStoreElement& elem);
        void EnterWorkspace(const PasswordStoreElements::PasswordStoreElement& elem);

        // Workspaces are merged into the listing by the manager rather than by PasswordStore:
        // they have no presence on disk, and the store deliberately knows nothing about them.
        [[nodiscard]] std::vector<PasswordStoreElements::PasswordStoreElement>
            CollectWorkspaceElements(const std::string& filter) const;

        bool TryDecryptPassword(const PasswordStoreElements::PasswordStoreElement & elem);
        bool TryDecryptTwoFactor(const PasswordStoreElements::PasswordStoreElement & elem);

        bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

        void SetRecursiveSearch(bool active);

        bool recursiveSearchActive = false;
        Glib::RefPtr<Gtk::CssProvider> recursiveSearchCssProvider;

        Glib::Dispatcher refreshDispatcher;

        // The actual passwordStore that manages the files and passwords.
        // This class is a GUI wrapper of this.
        PasswordStore passwordStore;

        Workspaces::WorkspaceDB workspaceDB;
        std::optional<std::pair<std::filesystem::path, std::string>> activeWorkspace;

        Gtk::SearchEntry searchEntry;

        PasswordStoreManagerTools tools;
        Folderview folderview;
        PasswordStoreOutputDisplay outputDisplay;
    };
}