#include <GUI/password_store_manager/components/PasswordStoreOutputDisplay.h>

#include <language/Language.h>
#include <system/Extensions.h>

#include "error/ClavisError.h"

namespace Clavis::GUI {
    PasswordStoreOutputDisplay::PasswordStoreOutputDisplay() :
        Gtk::Box(Gtk::Orientation::VERTICAL),
        outputHbox(Gtk::Orientation::HORIZONTAL)
    {
        set_margin_start(10);
        set_margin_end(10);
        set_margin_bottom(10);

        auto sep = Gtk::Separator(Gtk::Orientation::HORIZONTAL);
        append(sep);

        auto label = Gtk::Label(_(PASSWORDSTORE_DECRYPTED_PASSWORD_LABEL));
        label.set_margin_top(5);
        label.set_margin_bottom(5);
        append(label);

        outputTextBox.set_editable(false);
        outputTextBox.set_hexpand(true);
        outputTextBox.set_placeholder_text(_(PASSWORDSTORE_PASSWORD_OUTPUT_HINT));

        outputHbox.append(outputTextBox);


        copyButton.SetIcon(Icons::Actions::Copy);
        writeButton.SetIcon(Icons::Actions::Draw);
        showPasswordButton.SetIcons(Icons::Actions::Visible, Icons::Actions::Hidden);
        copyButton.set_margin_start(5);
        writeButton.set_margin_start(5);
        showPasswordButton.set_margin_start(5);
        outputHbox.append(copyButton);
        outputHbox.append(writeButton);
        outputHbox.append(showPasswordButton);

        SetPasswordVisibility();
        showPasswordButton.signal_clicked().connect([this]() {
            SetPasswordVisibility();
        });

        copyButton.signal_clicked().connect([this]() {
            if (!displayedPassword.IsDecrypted()) {
                DisplayError();
                return;
            }

            System::CopyToClipboard(displayedPassword.GetPassword());
        });

        writeButton.signal_clicked().connect([this]() {
            RaiseClavisError(_(ERROR_NOT_IMPLEMENTED));
        });

        append(outputHbox);
    }

    void PasswordStoreOutputDisplay::SetPasswordVisibility() {
        outputTextBox.set_visibility(showPasswordButton.GetState());
    }


    void PasswordStoreOutputDisplay::DisplayPassword(const Password &password) {
        displayedPassword = password;

        if (displayedPassword.IsDecrypted()) {
            outputTextBox.set_text(displayedPassword.GetPassword());
            outputTextBox.remove_css_class("error");
        }
    }

    void PasswordStoreOutputDisplay::DisplayError() {
        displayedPassword = Password(); // Clear the previously displayed password from memory for safety
        outputTextBox.set_text("");
        outputTextBox.add_css_class("error");

        Glib::signal_timeout().connect_once(
        [this]() {
            outputTextBox.remove_css_class("error");
        },
        3000);
    }



}
