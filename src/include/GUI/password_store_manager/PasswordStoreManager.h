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


    protected:

    private:
        void GoUp();
        void Chdir(const PasswordStoreElements::PasswordStoreElement& elem);

        bool TryDecryptPassword(const PasswordStoreElements::PasswordStoreElement & elem);

        bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

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