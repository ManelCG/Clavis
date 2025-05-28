#pragma once

#include <gtkmm.h>

#include <password_store/Password.h>

#include <GUI/components/IconButton.h>
#include <GUI/components/ToggleIconButton.h>

namespace Clavis::GUI {
    class PasswordStoreOutputDisplay : public Gtk::Box {
    public:
        PasswordStoreOutputDisplay();

        void DisplayPassword(const Password& password);
        void DisplayError();

    protected:

    private:
        void SetPasswordVisibility();

        Password displayedPassword;

        Gtk::Box outputHbox;
        Gtk::Entry outputTextBox;

        IconButton copyButton;
        IconButton writeButton;
        ToggleIconButton showPasswordButton;
    };
}
