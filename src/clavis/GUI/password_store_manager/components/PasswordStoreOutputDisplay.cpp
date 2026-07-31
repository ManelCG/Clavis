#include <GUI/password_store_manager/components/PasswordStoreOutputDisplay.h>

namespace Clavis::GUI {
    PasswordStoreOutputDisplay::PasswordStoreOutputDisplay()
        : Gtk::Box(Gtk::Orientation::VERTICAL) {
        append(passwordDisplay);
        append(twoFactorDisplay);
    }

    void PasswordStoreOutputDisplay::DisplayPassword(const Password& password) {
        ClearAll();
        passwordDisplay.DisplayPassword(password);
    }

    void PasswordStoreOutputDisplay::DisplayTwoFactor(const TwoFactor::TwoFactorEntry& entry,
                                                      const std::filesystem::path& path) {
        ClearAll();
        twoFactorDisplay.DisplayTwoFactor(entry, path);
    }

    void PasswordStoreOutputDisplay::DisplayPasswordError() {
        ClearAll();
        passwordDisplay.DisplayDecryptionError();
    }

    void PasswordStoreOutputDisplay::DisplayTwoFactorError() {
        ClearAll();
        twoFactorDisplay.DisplayDecryptionError();
    }

    void PasswordStoreOutputDisplay::ClearAll() {
        passwordDisplay.Clear();
        twoFactorDisplay.Clear();
    }

    void PasswordStoreOutputDisplay::TryCopyActive() {
        if (twoFactorDisplay.HasContent()) {
            twoFactorDisplay.TryCopy();
            return;
        }

        passwordDisplay.TryCopy();
    }

    void PasswordStoreOutputDisplay::SetOnAdvanceHotpCounter(
        const std::function<void(const std::filesystem::path&)>& callback) {
        twoFactorDisplay.SetOnAdvanceCounter(callback);
    }
}
