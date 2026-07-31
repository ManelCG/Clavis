#include <GUI/password_store_manager/components/TwoFactorOutputDisplay.h>

#include <extensions/GUIExtensions.h>
#include <language/Language.h>
#include <system/Extensions.h>

namespace Clavis::GUI {
    TwoFactorOutputDisplay::TwoFactorOutputDisplay()
        : OutputDisplay(_(PASSWORDSTORE_DECRYPTED_TWO_FACTOR_LABEL), _(PASSWORDSTORE_TWO_FACTOR_OUTPUT_HINT)),
          currentCounter(0),
          countdownBox(Gtk::Orientation::HORIZONTAL),
          hotpBox(Gtk::Orientation::HORIZONTAL),
          onAdvanceCounter([](const std::filesystem::path&) {}) {
        // A one-time code is meant to be read off the screen and typed, so it is always shown in
        // plaintext -- there is deliberately no visibility toggle here. The monospace styling is
        // applied per-state rather than once, so it never affects the placeholder text.

        countdownBar.set_min_value(0.0);
        countdownBar.set_max_value(1.0);
        countdownBar.set_size_request(60, -1);
        countdownBar.set_valign(Gtk::Align::CENTER);
        countdownLabel.set_width_chars(4);
        countdownBox.append(countdownLabel);
        countdownBox.append(countdownBar);
        countdownBar.set_margin_start(5);

        advanceButton.SetIcon(Icons::Actions::Refresh);
        advanceButton.set_tooltip_text(_(PASSWORDSTORE_TWO_FACTOR_NEXT_CODE_TOOLTIP));
        hotpCounterLabel.set_width_chars(5);
        hotpBox.append(hotpCounterLabel);
        hotpBox.append(advanceButton);
        advanceButton.set_margin_start(5);

        AppendTrailingWidget(countdownBox);
        AppendTrailingWidget(hotpBox);

        countdownBox.set_visible(false);
        hotpBox.set_visible(false);

        advanceButton.signal_clicked().connect([this]() {
            if (displayedEntry.GetType() != TwoFactor::OtpType::HOTP || sourcePath.empty())
                return;

            onAdvanceCounter(sourcePath);
        });

        // Routed through UniqueSignalTimeoutDispatcher (and therefore ConnectOnce, which checks
        // its `alive` flag) and re-armed from inside Tick, rather than using the repeating
        // Connect variant that lacks that guard.
        tickTimeout.SetMilliseconds(500);
        tickTimeout.SetAction([this]() {
            Tick();
        });
    }

    void TwoFactorOutputDisplay::SetOnAdvanceCounter(const std::function<void(const std::filesystem::path&)>& callback) {
        onAdvanceCounter = callback;
    }

    void TwoFactorOutputDisplay::DisplayTwoFactor(const TwoFactor::TwoFactorEntry& entry,
                                                  const std::filesystem::path& path) {
        displayedEntry = entry;
        sourcePath = path;

        if (!displayedEntry.IsValid()) {
            DisplayDecryptionError();
            return;
        }

        currentCounter = displayedEntry.GetCurrentTimeCounter();
        RefreshCode();

        if (!currentCode.empty()) {
            DisplaySuccess();
            ArmClearTimeout();
        }

        ApplyModeVisibility();
        Tick();
    }

    void TwoFactorOutputDisplay::DisplayDecryptionError() {
        Clear();

        set_visible(true);
        ShowDecryptionFailedPlaceholder();
        DisplayError();
    }

    void TwoFactorOutputDisplay::ApplyModeVisibility() {
        const bool isCounterBased = displayedEntry.GetType() == TwoFactor::OtpType::HOTP;

        countdownBox.set_visible(!isCounterBased);
        hotpBox.set_visible(isCounterBased);
    }

    void TwoFactorOutputDisplay::RefreshCode() {
        std::string code;
        if (!displayedEntry.TryGenerateCode(code)) {
            outputTextBox.remove_css_class("otp-code");
            SetOutputText("");
            return;
        }

        if (!currentCode.empty())
            System::SecureZero(currentCode.data(), currentCode.size());

        currentCode = code;
        outputTextBox.add_css_class("otp-code");
        SetOutputText(currentCode);
    }

    void TwoFactorOutputDisplay::Tick() {
        // Stops re-arming once the section is cleared or hidden, which is what ends the loop.
        if (!displayedEntry.IsValid() || !get_visible())
            return;

        if (displayedEntry.GetType() == TwoFactor::OtpType::HOTP) {
            hotpCounterLabel.set_text(_(PASSWORDSTORE_TWO_FACTOR_COUNTER_LABEL,
                                        std::to_string(displayedEntry.GetCounter())));
            return;   // Counter-based codes do not expire, so there is nothing to poll.
        }

        // Recomputed from the wall clock every tick rather than counted down, so the display
        // cannot drift away from the real time step.
        const auto counter = displayedEntry.GetCurrentTimeCounter();
        if (counter != currentCounter) {
            currentCounter = counter;
            RefreshCode();
        }

        const int remaining = displayedEntry.GetSecondsRemaining();
        const int period = displayedEntry.GetPeriod();

        countdownLabel.set_text(_(PASSWORDSTORE_TWO_FACTOR_SECONDS_REMAINING, std::to_string(remaining)));
        countdownBar.set_value(period > 0 ? static_cast<double>(remaining) / period : 0.0);

        tickTimeout.StartTimeout();
    }

    void TwoFactorOutputDisplay::OnClear() {
        outputTextBox.remove_css_class("otp-code");

        if (!currentCode.empty())
            System::SecureZero(currentCode.data(), currentCode.size());

        currentCode.clear();
        currentCounter = 0;
        sourcePath.clear();

        // Assigning a fresh entry runs the old one's destructor, zeroing the shared secret.
        displayedEntry = TwoFactor::TwoFactorEntry();

        countdownBox.set_visible(false);
        hotpBox.set_visible(false);
    }

    std::string TwoFactorOutputDisplay::GetClipboardText() const {
        return currentCode;
    }
}
