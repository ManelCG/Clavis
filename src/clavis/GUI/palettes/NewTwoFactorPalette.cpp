#include <GUI/palettes/NewTwoFactorPalette.h>

#include <crypto/Base32.h>
#include <extensions/StringHelper.h>
#include <language/Language.h>
#include <settings/Settings.h>
#include <system/Extensions.h>

namespace Clavis::GUI {
    namespace {
        constexpr int TYPE_INDEX_TOTP = 0;
        constexpr int TYPE_INDEX_HOTP = 1;
        constexpr int TYPE_INDEX_STEAM = 2;

        int AlgorithmToIndex(Crypto::HashAlgorithm algorithm) {
            switch (algorithm) {
                case Crypto::HashAlgorithm::SHA256: return 1;
                case Crypto::HashAlgorithm::SHA512: return 2;
                default: return 0;
            }
        }

        Crypto::HashAlgorithm IndexToAlgorithm(int index) {
            switch (index) {
                case 1: return Crypto::HashAlgorithm::SHA256;
                case 2: return Crypto::HashAlgorithm::SHA512;
                default: return Crypto::HashAlgorithm::SHA1;
            }
        }
    }

    NewTwoFactorPalette::NewTwoFactorPalette(const std::string& fixedName, const TwoFactor::TwoFactorEntry& initial)
        : DualChoicePalette(fixedName.empty() ? _(NEW_TWO_FACTOR_PALETTE_TITLE) : _(NEW_TWO_FACTOR_PALETTE_EDIT_TITLE)),
          isEditing(!fixedName.empty()),
          isSyncing(false),
          migrationUriPasted(false),
          workingEntry(initial),
          mainVBox(Gtk::Orientation::VERTICAL),
          columnsHBox(Gtk::Orientation::HORIZONTAL),
          columnsSeparator(Gtk::Orientation::VERTICAL),
          metadataSeparator(Gtk::Orientation::HORIZONTAL),
          leftVBox(Gtk::Orientation::VERTICAL),
          uriHBox(Gtk::Orientation::HORIZONTAL),
          rightVBox(Gtk::Orientation::VERTICAL),
          previewHBox(Gtk::Orientation::HORIZONTAL),
          metadataVBox(Gtk::Orientation::VERTICAL) {
        set_default_size(700, 620);

        // Unlike the fixed-size password dialog, this one holds a list that grows, so the user
        // needs to be able to make room for it.
        set_resizable(true);
        SetYesSuggested();

        titleLabel.set_text(isEditing ? _(NEW_TWO_FACTOR_PALETTE_EDIT_LABEL_TITLE)
                                      : _(NEW_TWO_FACTOR_PALETTE_LABEL_TITLE));
        titleLabel.set_margin(10);
        mainVBox.append(titleLabel);

        BuildLeftColumn();
        BuildRightColumn();

        columnsSeparator.set_margin_start(10);
        columnsSeparator.set_margin_end(10);
        columnsHBox.append(leftVBox);
        columnsHBox.append(columnsSeparator);
        columnsHBox.append(rightVBox);

        // Content is inset from the window edge while the button row stays flush, matching the
        // new-password dialog.
        columnsHBox.set_margin_start(10);
        columnsHBox.set_margin_end(10);
        mainVBox.append(columnsHBox);

        metadataSeparator.set_margin_top(15);
        metadataSeparator.set_margin_bottom(15);
        mainVBox.append(metadataSeparator);

        BuildMetadataSection();
        metadataVBox.set_margin_start(10);
        metadataVBox.set_margin_end(10);

        // Breathing room between the content and the Yes/No row at the bottom.
        metadataVBox.set_margin_bottom(20);
        mainVBox.append(metadataVBox);

        append(mainVBox);

        if (isEditing) {
            nameEntry.set_text(fixedName);
            nameEntry.set_sensitive(false);
            nameEntry.set_editable(false);
            nameLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_CURRENT_NAME_LABEL));
        }

        PopulateFieldsFrom(workingEntry);

        // The preview is the validation affordance: the user compares it against the site or
        // their phone before committing. Re-armed from inside its own action.
        previewTimeout.SetMilliseconds(500);
        previewTimeout.SetAction([this]() {
            UpdatePreview();

            if (get_visible())
                previewTimeout.StartTimeout();
        });
        previewTimeout.StartTimeout();
    }

#pragma region LAYOUT
    void NewTwoFactorPalette::BuildLeftColumn() {
        leftVBox.set_hexpand(true);

        // Labels are left with their default alignment so they centre over their field, matching
        // the new-password dialog.
        nameLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_NAME_LABEL));
        leftVBox.append(nameLabel);

        // Same rhythm as the new-password dialog: a small gap binds a label to its field, a
        // larger one separates the groups.
        nameEntry.set_margin_top(10);
        leftVBox.append(nameEntry);

        uriLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_URI_LABEL));
        uriLabel.set_margin_top(25);
        leftVBox.append(uriLabel);

        uriEntry.set_hexpand(true);
        uriHBox.set_margin_top(10);
        pasteButton.SetIcon(Icons::Actions::Import);
        pasteButton.set_tooltip_text(_(NEW_TWO_FACTOR_PALETTE_PASTE_TOOLTIP));
        pasteButton.set_margin_start(5);
        uriHBox.append(uriEntry);
        uriHBox.append(pasteButton);
        leftVBox.append(uriHBox);

        // Issuer and account are how the entry identifies itself in other apps, so they belong
        // next to the name rather than buried with the protocol parameters.
        issuerLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_ISSUER_OPTIONAL_LABEL));
        issuerLabel.set_halign(Gtk::Align::START);
        accountLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_ACCOUNT_OPTIONAL_LABEL));
        accountLabel.set_halign(Gtk::Align::START);

        issuerEntry.set_hexpand(true);
        accountEntry.set_hexpand(true);

        identityGrid.set_row_spacing(4);
        identityGrid.set_column_spacing(8);
        identityGrid.set_margin_top(25);
        identityGrid.attach(issuerLabel, 0, 0);
        identityGrid.attach(issuerEntry, 1, 0);
        identityGrid.attach(accountLabel, 0, 1);
        identityGrid.attach(accountEntry, 1, 1);
        leftVBox.append(identityGrid);

        previewLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_PREVIEW_INVALID));
        previewLabel.set_wrap(true);
        previewLabel.set_hexpand(true);

        // The code has to reach the provider's "enter the passcode" box to finish enrolment, so
        // it is copyable straight from here rather than only after saving.
        copyPreviewButton.SetIcon(Icons::Actions::Copy);
        copyPreviewButton.set_tooltip_text(_(NEW_TWO_FACTOR_PALETTE_COPY_CODE_TOOLTIP));
        copyPreviewButton.set_margin_start(5);
        copyPreviewButton.set_sensitive(false);

        previewHBox.append(previewLabel);
        previewHBox.append(copyPreviewButton);
        previewHBox.set_margin_top(20);
        leftVBox.append(previewHBox);

        clipboardClearTimeout.SetAction([]() {
            Gdk::Display::get_default()->get_clipboard()->set_text("");
        });

        copyPreviewButton.signal_clicked().connect([this]() {
            if (previewCode.empty() || !System::CopyToClipboard(previewCode))
                return;

            const int clearSeconds = Settings::CLIPBOARD_CLEAR_SECONDS.GetValue();
            if (clearSeconds <= 0)
                return;

            clipboardClearTimeout.SetSeconds(clearSeconds);
            clipboardClearTimeout.StartTimeout();
        });

        uriEntry.signal_changed().connect([this]() {
            SyncFromUri();
        });

        issuerEntry.signal_changed().connect([this]() {
            SyncFromFields();
        });
        accountEntry.signal_changed().connect([this]() {
            SyncFromFields();
        });

        pasteButton.signal_clicked().connect([this]() {
            const auto clipboard = Gdk::Display::get_default()->get_clipboard();
            clipboard->read_text_async([this, clipboard](Glib::RefPtr<Gio::AsyncResult>& result) {
                try {
                    uriEntry.set_text(clipboard->read_text_finish(result));
                } catch (const Glib::Error&) {
                    uriEntry.DisplayError();
                }
            });
        });
    }

    void NewTwoFactorPalette::BuildRightColumn() {
        rightVBox.set_hexpand(true);

        detailsLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_DETAILS_LABEL));
        rightVBox.append(detailsLabel);

        detailsGrid.set_row_spacing(4);
        detailsGrid.set_column_spacing(8);
        detailsGrid.set_margin_top(10);

        typeCombo.append("TOTP (time-based)");
        typeCombo.append("HOTP (counter-based)");
        typeCombo.append("Steam");
        typeCombo.set_active(TYPE_INDEX_TOTP);

        algorithmCombo.append("SHA1");
        algorithmCombo.append("SHA256");
        algorithmCombo.append("SHA512");
        algorithmCombo.set_active(0);

        // The secret is never typed here -- it arrives through the paste box, whether as a bare
        // secret or extracted from a URI. Read-only means it cannot be damaged by a stray
        // keystroke, and a damaged secret is unrecoverable.
        secretEntry.set_editable(false);
        secretEntry.set_hexpand(true);
        secretEntry.add_css_class("readonly-entry");

        const auto addRow = [this](int row, Gtk::Label& label, const Glib::ustring& text, Gtk::Widget& field) {
            label.set_text(text);
            label.set_halign(Gtk::Align::START);
            detailsGrid.attach(label, 0, row);
            field.set_hexpand(true);
            detailsGrid.attach(field, 1, row);
        };

        int row = 0;
        addRow(row++, typeLabel, _(NEW_TWO_FACTOR_PALETTE_TYPE_LABEL), typeCombo);
        addRow(row++, secretLabel, _(NEW_TWO_FACTOR_PALETTE_SECRET_LABEL), secretEntry);
        addRow(row++, algorithmLabel, _(NEW_TWO_FACTOR_PALETTE_ALGORITHM_LABEL), algorithmCombo);
        addRow(row++, digitsLabel, _(NEW_TWO_FACTOR_PALETTE_DIGITS_LABEL), digitsEntry);
        addRow(row++, periodLabel, _(NEW_TWO_FACTOR_PALETTE_PERIOD_LABEL), periodEntry);
        addRow(row++, counterLabel, _(NEW_TWO_FACTOR_PALETTE_COUNTER_LABEL), counterEntry);

        rightVBox.append(detailsGrid);

        const auto onFieldChanged = [this]() {
            SyncFromFields();
        };

        secretEntry.signal_changed().connect(onFieldChanged);
        digitsEntry.signal_changed().connect(onFieldChanged);
        periodEntry.signal_changed().connect(onFieldChanged);
        counterEntry.signal_changed().connect(onFieldChanged);
        algorithmCombo.signal_changed().connect(onFieldChanged);
        typeCombo.signal_changed().connect([this]() {
            ApplyTypeVisibility();
            SyncFromFields();
        });

        secretEntry.SetCorrectnessCheck([this]() {
            std::vector<uint8_t> decoded;
            return Crypto::Base32::TryDecode(secretEntry.get_text(), decoded) && !decoded.empty();
        });
    }

    void NewTwoFactorPalette::BuildMetadataSection() {
        recoveryLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_RECOVERY_LABEL));
        recoveryLabel.set_halign(Gtk::Align::START);
        metadataVBox.append(recoveryLabel);

        // The list brings its own scroll area, so it only needs a floor and room to grow.
        recoveryCodeList.set_size_request(-1, 130);
        recoveryCodeList.set_margin_top(5);
        recoveryCodeList.set_vexpand(true);
        metadataVBox.append(recoveryCodeList);

        notesLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_NOTES_LABEL));
        notesLabel.set_halign(Gtk::Align::START);
        notesLabel.set_margin_top(10);
        metadataVBox.append(notesLabel);

        notesView.set_wrap_mode(Gtk::WrapMode::WORD_CHAR);
        notesScroll.set_child(notesView);
        notesScroll.set_size_request(-1, 80);
        notesScroll.set_has_frame(true);
        notesScroll.set_margin_top(5);
        notesScroll.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
        metadataVBox.append(notesScroll);
    }
#pragma endregion

#pragma region SYNCHRONISATION
    TwoFactor::OtpType NewTwoFactorPalette::GetSelectedType() const {
        return typeCombo.get_active_row_number() == TYPE_INDEX_HOTP ? TwoFactor::OtpType::HOTP
                                                                    : TwoFactor::OtpType::TOTP;
    }

    TwoFactor::OtpEncoder NewTwoFactorPalette::GetSelectedEncoder() const {
        return typeCombo.get_active_row_number() == TYPE_INDEX_STEAM ? TwoFactor::OtpEncoder::STEAM
                                                                     : TwoFactor::OtpEncoder::DEFAULT;
    }

    void NewTwoFactorPalette::ApplyTypeVisibility() {
        const bool isCounterBased = GetSelectedType() == TwoFactor::OtpType::HOTP;
        const bool isSteam = GetSelectedEncoder() == TwoFactor::OtpEncoder::STEAM;

        // Period and counter describe mutually exclusive schemes, so only the applicable one is
        // shown. Hiding both widgets in a row makes the grid collapse it entirely.
        periodLabel.set_visible(!isCounterBased);
        periodEntry.set_visible(!isCounterBased);
        counterLabel.set_visible(isCounterBased);
        counterEntry.set_visible(isCounterBased);

        // Steam's parameters are fixed by the scheme; exposing them as editable would only
        // invite a configuration that cannot produce valid codes.
        periodEntry.set_sensitive(!isSteam);
        digitsEntry.set_sensitive(!isSteam);
        algorithmCombo.set_sensitive(!isSteam);
    }

    void NewTwoFactorPalette::PopulateDetailsFrom(const TwoFactor::TwoFactorEntry& entry) {
        isSyncing = true;

        if (entry.GetEncoder() == TwoFactor::OtpEncoder::STEAM)
            typeCombo.set_active(TYPE_INDEX_STEAM);
        else
            typeCombo.set_active(entry.GetType() == TwoFactor::OtpType::HOTP ? TYPE_INDEX_HOTP : TYPE_INDEX_TOTP);

        issuerEntry.set_text(entry.GetIssuer());
        accountEntry.set_text(entry.GetAccount());
        secretEntry.set_text(entry.GetSecretBase32());
        algorithmCombo.set_active(AlgorithmToIndex(entry.GetAlgorithm()));
        digitsEntry.SetValue(entry.GetDigits());
        periodEntry.SetValue(entry.GetPeriod());
        counterEntry.SetValue(static_cast<int>(entry.GetCounter()));

        isSyncing = false;

        ApplyTypeVisibility();
        UpdatePreview();
    }

    void NewTwoFactorPalette::PopulateFieldsFrom(const TwoFactor::TwoFactorEntry& entry) {
        PopulateDetailsFrom(entry);

        recoveryCodeList.SetCodes(entry.GetRecoveryCodes());
        notesView.get_buffer()->set_text(entry.GetNotes());

        if (entry.IsValid()) {
            isSyncing = true;
            uriEntry.set_text(entry.BuildUri());
            isSyncing = false;
        }
    }

    void NewTwoFactorPalette::SyncFromUri() {
        if (isSyncing)
            return;

        const auto text = StringHelper::Trim(uriEntry.get_text());
        if (text.empty())
            return;

        // A bulk-export URI pasted here is almost certainly a user trying to migrate a whole
        // library, so hand it straight to the importer instead of rejecting it.
        if (StringHelper::ToLower(text).rfind(TwoFactor::OTPAUTH_MIGRATION_SCHEME, 0) == 0) {
            migrationUriPasted = true;
            migrationUri = text;
            DualChoicePalette::DoGiveResponse(true);
            return;
        }

        TwoFactor::TwoFactorEntry parsed;
        if (TwoFactor::TwoFactorEntry::TryFromUri(text, parsed)) {
            // A URI describes the whole credential, so every field follows from it.
            // Metadata lives outside the URI and has to survive the re-parse.
            parsed.SetMetadata(workingEntry.GetMetadata());
            workingEntry = parsed;

            PopulateDetailsFrom(parsed);

            // Pasting fills in a sensible name, but never overwrites one the user chose.
            if (!isEditing && nameEntry.get_text().empty())
                nameEntry.set_text(parsed.GetSuggestedName());

            uriEntry.DisplaySuccess();
            return;
        }

        // Not a URI, so it is the bare secret. Providers that only show you a secret are
        // implicitly saying "use the defaults", which is what the untouched fields already hold.
        // Deliberately no error styling here: this runs on every keystroke, and a half-typed
        // secret is not a mistake. Validation on accept reports a genuinely bad one.
        secretEntry.set_text(text);
    }

    void NewTwoFactorPalette::SyncFromFields() {
        if (isSyncing)
            return;

        TwoFactor::TwoFactorEntry entry;
        entry.SetMetadata(workingEntry.GetMetadata());

        entry.SetType(GetSelectedType());
        entry.SetEncoder(GetSelectedEncoder());
        entry.SetIssuer(issuerEntry.get_text());
        entry.SetAccount(accountEntry.get_text());

        if (GetSelectedEncoder() == TwoFactor::OtpEncoder::STEAM) {
            entry.SetAlgorithm(Crypto::HashAlgorithm::SHA1);
            entry.SetDigits(TwoFactor::STEAM_DIGITS);
            entry.SetPeriod(TwoFactor::STEAM_PERIOD);
        } else {
            entry.SetAlgorithm(IndexToAlgorithm(algorithmCombo.get_active_row_number()));
            entry.SetDigits(digitsEntry.GetValue());
            entry.SetPeriod(periodEntry.GetValue());
        }

        entry.SetCounter(static_cast<uint64_t>(std::max(0, counterEntry.GetValue())));

        // An undecodable secret is kept in the model as-is; accept-time validation reports it.
        entry.TrySetSecretBase32(secretEntry.get_text());

        // Deliberately does not write back to the paste box. It is an input, not a mirror --
        // rewriting it would replace whatever the user pasted mid-edit.
        workingEntry = entry;

        UpdatePreview();
    }

    void NewTwoFactorPalette::UpdatePreview() {
        std::string code;
        if (!workingEntry.IsValid() || !workingEntry.TryGenerateCode(code)) {
            previewCode.clear();
            copyPreviewButton.set_sensitive(false);
            previewLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_PREVIEW_INVALID));
            return;
        }

        previewCode = code;
        copyPreviewButton.set_sensitive(true);

        if (workingEntry.GetType() == TwoFactor::OtpType::HOTP) {
            previewLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_PREVIEW_LABEL, code));
            return;
        }

        previewLabel.set_text(_(NEW_TWO_FACTOR_PALETTE_PREVIEW_LABEL,
                                code + "  (" + std::to_string(workingEntry.GetSecondsRemaining()) + "s)"));
    }
#pragma endregion

#pragma region RESULT
    std::string NewTwoFactorPalette::GetName() const {
        return nameEntry.get_text();
    }

    TwoFactor::TwoFactorEntry NewTwoFactorPalette::GetEntry() const {
        auto entry = workingEntry;

        entry.SetRecoveryCodes(recoveryCodeList.GetCodes());
        entry.SetNotes(StringHelper::Trim(notesView.get_buffer()->get_text()));

        return entry;
    }

    bool NewTwoFactorPalette::WasMigrationUriPasted() const {
        return migrationUriPasted;
    }

    std::string NewTwoFactorPalette::GetMigrationUri() const {
        return migrationUri;
    }

    bool NewTwoFactorPalette::ShouldSubmitOnReturn() const {
        // Return inside the recovery-code list means "add this code", not "accept the dialog".
        const auto* focus = get_focus();

        // is_ancestor takes a non-const reference; the call itself does not mutate.
        return focus == nullptr || !focus->is_ancestor(const_cast<RecoveryCodeList&>(recoveryCodeList));
    }

    void NewTwoFactorPalette::DoGiveResponse(bool r) {
        if (!r) {
            DualChoicePalette::DoGiveResponse(false);
            return;
        }

        bool success = true;

        if (!nameEntry.IsValid()) {
            success = false;
            nameEntry.DisplayError();
        }

        // The secret box is read-only, so a bad secret is reported on the paste box that fed it,
        // which is the field the user can actually correct.
        if (!workingEntry.IsValid()) {
            success = false;
            uriEntry.DisplayError();
            secretEntry.DisplayError();
        }

        if (!success)
            return;

        DualChoicePalette::DoGiveResponse(true);
    }
#pragma endregion
}
