#pragma once

#include <filesystem>
#include <functional>

#include <two_factor/TwoFactorEntry.h>

#include <GUI/password_store_manager/components/OutputDisplay.h>

namespace Clavis::GUI {
    class TwoFactorOutputDisplay : public OutputDisplay {
    public:
        TwoFactorOutputDisplay();

        void DisplayTwoFactor(const TwoFactor::TwoFactorEntry& entry, const std::filesystem::path& path);
        void DisplayDecryptionError();

        // Invoked when the user asks for the next HOTP code. The workflow re-reads the file,
        // advances the counter, re-encrypts and commits.
        void SetOnAdvanceCounter(const std::function<void(const std::filesystem::path&)>& callback);

    protected:
        void OnClear() override;
        [[nodiscard]] std::string GetClipboardText() const override;

    private:
        void Tick();
        void RefreshCode();
        void ApplyModeVisibility();

        TwoFactor::TwoFactorEntry displayedEntry;
        std::filesystem::path sourcePath;

        std::string currentCode;
        uint64_t currentCounter;

        // Time-based entries: countdown until the code rolls over.
        Gtk::Box countdownBox;
        Gtk::Label countdownLabel;
        Gtk::LevelBar countdownBar;

        // Counter-based entries: HOTP codes never expire, so there is nothing to count down.
        // The user advances the counter explicitly instead.
        Gtk::Box hotpBox;
        Gtk::Label hotpCounterLabel;
        IconButton advanceButton;

        UniqueSignalTimeoutDispatcher tickTimeout;

        std::function<void(const std::filesystem::path&)> onAdvanceCounter;
    };
}
