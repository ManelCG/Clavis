#include <GUI/palettes/TwoFactorDetailsPalette.h>

#include <crypto/Hash.h>
#include <language/Language.h>
#include <settings/Settings.h>
#include <system/Extensions.h>

namespace Clavis::GUI {
    TwoFactorDetailsPalette::TwoFactorDetailsPalette(const std::string& name, const TwoFactor::TwoFactorEntry& entry)
        : Palette(_(TWO_FACTOR_DETAILS_PALETTE_TITLE, name)),
          entry(entry),
          mainVBox(Gtk::Orientation::VERTICAL),
          recoveryVBox(Gtk::Orientation::VERTICAL) {
        set_default_size(420, 480);
        recoveryVBox.set_valign(Gtk::Align::START);

        mainVBox.set_margin(10);

        parameterGrid.set_row_spacing(4);
        parameterGrid.set_column_spacing(12);

        int row = 0;
        const bool isSteam = entry.GetEncoder() == TwoFactor::OtpEncoder::STEAM;
        const bool isCounterBased = entry.GetType() == TwoFactor::OtpType::HOTP;

        AddParameterRow(row++, _(NEW_TWO_FACTOR_PALETTE_TYPE_LABEL),
                        isSteam ? "Steam" : (isCounterBased ? "HOTP" : "TOTP"));
        AddParameterRow(row++, _(NEW_TWO_FACTOR_PALETTE_ISSUER_LABEL), entry.GetIssuer());
        AddParameterRow(row++, _(NEW_TWO_FACTOR_PALETTE_ACCOUNT_LABEL), entry.GetAccount());
        AddParameterRow(row++, _(NEW_TWO_FACTOR_PALETTE_ALGORITHM_LABEL),
                        Crypto::HashAlgorithmToString(entry.GetAlgorithm()));
        AddParameterRow(row++, _(NEW_TWO_FACTOR_PALETTE_DIGITS_LABEL), std::to_string(entry.GetDigits()));

        if (isCounterBased)
            AddParameterRow(row++, _(NEW_TWO_FACTOR_PALETTE_COUNTER_LABEL), std::to_string(entry.GetCounter()));
        else
            AddParameterRow(row++, _(NEW_TWO_FACTOR_PALETTE_PERIOD_LABEL), std::to_string(entry.GetPeriod()));

        mainVBox.append(parameterGrid);

        auto parameterSeparator = std::make_unique<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
        parameterSeparator->set_margin_top(10);
        parameterSeparator->set_margin_bottom(10);
        mainVBox.append(*parameterSeparator);
        ownedWidgets.push_back(std::move(parameterSeparator));

        recoveryHeaderLabel.set_text(_(TWO_FACTOR_DETAILS_PALETTE_RECOVERY_LABEL));
        recoveryHeaderLabel.set_halign(Gtk::Align::START);
        mainVBox.append(recoveryHeaderLabel);

        const auto recoveryCodes = entry.GetRecoveryCodes();
        if (recoveryCodes.empty()) {
            auto empty = std::make_unique<Gtk::Label>(_(TWO_FACTOR_DETAILS_PALETTE_NO_RECOVERY));
            empty->set_halign(Gtk::Align::START);
            empty->set_wrap(true);
            empty->set_margin_top(5);
            recoveryVBox.append(*empty);
            ownedWidgets.push_back(std::move(empty));
        } else {
            for (size_t i = 0; i < recoveryCodes.size(); i++)
                AddRecoveryRow(i + 1, recoveryCodes[i]);
        }

        recoveryScroll.set_child(recoveryVBox);
        recoveryScroll.set_size_request(-1, recoveryCodes.empty() ? 40 : 120);
        recoveryScroll.set_margin_top(5);

        // Absorbs any extra height when the window is resized, so growing the window grows the
        // code list instead of leaving a gap.
        recoveryScroll.set_vexpand(true);
        mainVBox.append(recoveryScroll);

        if (const auto notes = entry.GetNotes(); !notes.empty()) {
            notesHeaderLabel.set_text(_(TWO_FACTOR_DETAILS_PALETTE_NOTES_LABEL));
            notesHeaderLabel.set_halign(Gtk::Align::START);
            notesHeaderLabel.set_margin_top(10);
            mainVBox.append(notesHeaderLabel);

            notesLabel.set_text(notes);
            notesLabel.set_halign(Gtk::Align::START);
            notesLabel.set_wrap(true);
            notesLabel.set_selectable(true);
            notesLabel.set_margin_top(5);
            notesLabel.set_vexpand(false);
            mainVBox.append(notesLabel);
        }

        clipboardClearTimeout.SetAction([]() {
            Gdk::Display::get_default()->get_clipboard()->set_text("");
        });

        set_child(mainVBox);
    }

    void TwoFactorDetailsPalette::AddParameterRow(int row, const Glib::ustring& label, const std::string& value) {
        auto labelWidget = std::make_unique<Gtk::Label>(label);
        labelWidget->set_halign(Gtk::Align::START);

        auto valueWidget = std::make_unique<Gtk::Label>(value.empty() ? "-" : value);
        valueWidget->set_halign(Gtk::Align::START);
        valueWidget->set_selectable(true);
        valueWidget->set_hexpand(true);

        parameterGrid.attach(*labelWidget, 0, row);
        parameterGrid.attach(*valueWidget, 1, row);

        ownedWidgets.push_back(std::move(labelWidget));
        ownedWidgets.push_back(std::move(valueWidget));
    }

    void TwoFactorDetailsPalette::AddRecoveryRow(size_t oneBasedIndex, const std::string& code) {
        auto rowBox = std::make_unique<Gtk::Box>(Gtk::Orientation::HORIZONTAL);

        // Numbered from 1 so the list doubles as a count of how many codes are left.
        auto indexLabel = std::make_unique<Gtk::Label>(Glib::ustring(std::to_string(oneBasedIndex)) + ".");
        indexLabel->set_width_chars(4);
        indexLabel->set_xalign(1.0f);
        indexLabel->set_margin_end(5);
        indexLabel->add_css_class("dim-label");

        auto codeLabel = std::make_unique<Gtk::Label>(code);
        codeLabel->set_halign(Gtk::Align::START);
        codeLabel->set_hexpand(true);
        codeLabel->set_selectable(true);

        auto copyButton = std::make_unique<IconButton>(Icons::Actions::Copy);
        copyButton->signal_clicked().connect([this, code]() {
            if (!System::CopyToClipboard(code))
                return;

            // Recovery codes get the same clipboard hygiene as passwords and OTP codes.
            const int clearSeconds = Settings::CLIPBOARD_CLEAR_SECONDS.GetValue();
            if (clearSeconds <= 0)
                return;

            clipboardClearTimeout.SetSeconds(clearSeconds);
            clipboardClearTimeout.StartTimeout();
        });

        rowBox->append(*indexLabel);
        rowBox->append(*codeLabel);
        rowBox->append(*copyButton);
        rowBox->set_margin_bottom(2);

        recoveryVBox.append(*rowBox);

        ownedWidgets.push_back(std::move(indexLabel));
        ownedWidgets.push_back(std::move(codeLabel));
        ownedWidgets.push_back(std::move(copyButton));
        ownedWidgets.push_back(std::move(rowBox));
    }
}
