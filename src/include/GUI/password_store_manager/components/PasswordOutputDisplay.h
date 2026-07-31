#pragma once

#include <password_store/Password.h>

#include <GUI/components/ToggleIconButton.h>
#include <GUI/password_store_manager/components/OutputDisplay.h>

namespace Clavis::GUI {
    class PasswordOutputDisplay : public OutputDisplay {
    public:
        PasswordOutputDisplay();

        void DisplayPassword(const Password& password);
        void DisplayDecryptionError();

    protected:
        void OnClear() override;
        [[nodiscard]] std::string GetClipboardText() const override;

    private:
        void ApplyPasswordVisibility();

        ToggleIconButton showPasswordButton;
        Password displayedPassword;
    };
}
