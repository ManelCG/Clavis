#pragma once

#include <gtkmm.h>

#include <password_store/PasswordStore.h>

#include <GUI/palettes/Palette.h>

namespace Clavis::GUI {
    class PasswordStoreDataPalette : public Palette{
    public:
        PasswordStoreDataPalette(const PasswordStore& passwordStore);
    protected:

    private:
        Gtk::Box mainVBox;

        Gtk::Label passwordStoreDirectoryLabel;
        Gtk::Entry passwordStoreDirectoryEntry;

        Gtk::Box tableHBox;
        Gtk::Box tableLabelsVBox;
        Gtk::Box tableValuesVBox;

        Gtk::Label numPasswordsLabel;
        Gtk::Label numPasswordsValueLabel;

        Gtk::Label numFoldersLabel;
        Gtk::Label numFoldersValueLabel;

        PasswordStore store;
    };
}