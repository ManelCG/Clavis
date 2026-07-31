#pragma once

#include <filesystem>
#include <functional>

#include <gtkmm.h>

#include <password_store/Password.h>
#include <two_factor/TwoFactorEntry.h>

#include <GUI/password_store_manager/components/PasswordOutputDisplay.h>
#include <GUI/password_store_manager/components/TwoFactorOutputDisplay.h>

namespace Clavis::GUI {
    // Owns both decrypted-output sections and guarantees that only one is ever populated.
    //
    // Every Display* entry point clears both sections first, so "selecting a new item wipes the
    // previous one" is enforced in exactly one place and holds for password -> password,
    // 2FA -> 2FA and both cross transitions alike.
    class PasswordStoreOutputDisplay : public Gtk::Box {
    public:
        PasswordStoreOutputDisplay();

        void DisplayPassword(const Password& password);
        void DisplayTwoFactor(const TwoFactor::TwoFactorEntry& entry, const std::filesystem::path& path);

        // Reports a failed decryption against whichever kind of item was being opened.
        void DisplayPasswordError();
        void DisplayTwoFactorError();

        void ClearAll();

        // Copies from whichever section currently holds something.
        void TryCopyActive();

        void SetOnAdvanceHotpCounter(const std::function<void(const std::filesystem::path&)>& callback);

    protected:

    private:
        PasswordOutputDisplay passwordDisplay;
        TwoFactorOutputDisplay twoFactorDisplay;
    };
}
