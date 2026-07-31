#pragma once

#include <gtkmm.h>

#include <filesystem>

#include <password_store/PasswordStore.h>

#include <GUI/password_store_manager/components/PasswordStoreFolderview.h>
#include <GUI/password_store_manager/components/PasswordStoreManagerTools.h>
#include <GUI/password_store_manager/components/PasswordStoreOutputDisplay.h>


namespace Clavis::GUI {
    class PasswordStoreManager : public Gtk::Box {
    public:
        PasswordStoreManager();

        PasswordStore GetPasswordStore() const;

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

        Gtk::SearchEntry searchEntry;

        PasswordStoreManagerTools tools;
        Folderview folderview;
        PasswordStoreOutputDisplay outputDisplay;
    };
}