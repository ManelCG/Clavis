#include <GUI/password_store_manager/components/PasswordOutputDisplay.h>

#include <language/Language.h>

namespace Clavis::GUI {
    PasswordOutputDisplay::PasswordOutputDisplay()
        : OutputDisplay(_(PASSWORDSTORE_DECRYPTED_PASSWORD_LABEL), _(PASSWORDSTORE_PASSWORD_OUTPUT_HINT)) {
        showPasswordButton.SetIcons(Icons::Actions::Visible, Icons::Actions::Hidden);
        AppendTrailingWidget(showPasswordButton);

        ApplyPasswordVisibility();
        showPasswordButton.signal_clicked().connect([this]() {
            ApplyPasswordVisibility();
        });
    }

    void PasswordOutputDisplay::ApplyPasswordVisibility() {
        outputTextBox.set_visibility(showPasswordButton.GetState());
    }

    void PasswordOutputDisplay::DisplayPassword(const Password& password) {
        displayedPassword = password;

        if (!displayedPassword.IsDecrypted())
            return;

        SetOutputText(displayedPassword.GetPassword());
        DisplaySuccess();
        ArmClearTimeout();
    }

    void PasswordOutputDisplay::DisplayDecryptionError() {
        Clear();

        set_visible(true);
        ShowDecryptionFailedPlaceholder();
        DisplayError();
    }

    void PasswordOutputDisplay::OnClear() {
        // Assigning a fresh Password runs the old one's destructor, which securely zeroes it.
        displayedPassword = Password();
    }

    std::string PasswordOutputDisplay::GetClipboardText() const {
        if (!displayedPassword.IsDecrypted())
            return "";

        return displayedPassword.GetPassword();
    }
}
