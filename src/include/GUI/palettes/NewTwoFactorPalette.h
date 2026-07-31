#pragma once

#include <string>

#include <gtkmm.h>

#include <two_factor/TwoFactorEntry.h>

#include <GUI/components/IconButton.h>
#include <GUI/components/RecoveryCodeList.h>
#include <GUI/components/RequiredEntry.h>
#include <GUI/components/typedEntries/IntEntry.h>
#include <GUI/palettes/DualChoicePalette.h>
#include <GUI/signals/UniqueSignalTimeoutDispatcher.h>

namespace Clavis::GUI {
    // Adds or edits a single 2FA credential.
    //
    // A 2FA secret is always issued by the relying party, never invented locally, so there is
    // deliberately no "generate" button here.
    //
    // The paste box accepts either form the provider might give you: a full otpauth:// URI, or
    // the bare Base32 secret on its own. A URI carries the algorithm, digit count and period; a
    // bare secret carries none of those, so it falls back to the SHA1/6/30 defaults that nearly
    // every provider uses. Input flows one way, box -> fields: editing a field never rewrites the
    // box, so what was pasted is never silently replaced.
    class NewTwoFactorPalette : public DualChoicePalette<NewTwoFactorPalette> {
    public:
        // An empty fixedName means "create"; a non-empty one means "edit" and locks the name.
        NewTwoFactorPalette(const std::string& fixedName, const TwoFactor::TwoFactorEntry& initial);

        [[nodiscard]] std::string GetName() const;
        [[nodiscard]] TwoFactor::TwoFactorEntry GetEntry() const;

        // True when the user pasted a bulk-export URI, which the workflow hands to the importer.
        [[nodiscard]] bool WasMigrationUriPasted() const;
        [[nodiscard]] std::string GetMigrationUri() const;

    protected:
        void DoGiveResponse(bool r) override;
        [[nodiscard]] bool ShouldSubmitOnReturn() const override;

    private:
        void BuildLeftColumn();
        void BuildRightColumn();
        void BuildMetadataSection();

        // Reads the paste box: a URI populates every field, anything else is taken as the raw
        // secret. isSyncing suppresses the field handlers while they are being written to.
        void SyncFromUri();
        void SyncFromFields();
        void PopulateFieldsFrom(const TwoFactor::TwoFactorEntry& entry);
        void PopulateDetailsFrom(const TwoFactor::TwoFactorEntry& entry);

        // Period and counter are mutually exclusive, so the inapplicable one is hidden outright
        // rather than left visible and greyed.
        void ApplyTypeVisibility();
        void UpdatePreview();

        [[nodiscard]] TwoFactor::OtpType GetSelectedType() const;
        [[nodiscard]] TwoFactor::OtpEncoder GetSelectedEncoder() const;

        bool isEditing;
        bool isSyncing;
        bool migrationUriPasted;
        std::string migrationUri;

        TwoFactor::TwoFactorEntry workingEntry;

        Gtk::Label titleLabel;
        Gtk::Box mainVBox;
        Gtk::Box columnsHBox;
        Gtk::Separator columnsSeparator;
        Gtk::Separator metadataSeparator;

        Gtk::Box leftVBox;
        Gtk::Label nameLabel;
        RequiredEntry nameEntry;
        Gtk::Label uriLabel;
        Gtk::Box uriHBox;
        RequiredEntry uriEntry;
        IconButton pasteButton;
        // A grid rather than two boxes so the two entries line up despite differing label widths.
        Gtk::Grid identityGrid;
        Gtk::Label issuerLabel;
        Gtk::Entry issuerEntry;
        Gtk::Label accountLabel;
        Gtk::Entry accountEntry;

        Gtk::Box previewHBox;
        Gtk::Label previewLabel;
        IconButton copyPreviewButton;

        // The previewed code on its own, without the surrounding label text.
        std::string previewCode;

        Gtk::Box rightVBox;
        Gtk::Label detailsLabel;
        Gtk::Grid detailsGrid;
        Gtk::Label typeLabel;
        Gtk::ComboBoxText typeCombo;
        Gtk::Label secretLabel;
        RequiredEntry secretEntry;
        Gtk::Label algorithmLabel;
        Gtk::ComboBoxText algorithmCombo;
        Gtk::Label digitsLabel;
        IntEntry digitsEntry;
        Gtk::Label periodLabel;
        IntEntry periodEntry;
        Gtk::Label counterLabel;
        IntEntry counterEntry;

        Gtk::Box metadataVBox;
        Gtk::Label recoveryLabel;
        RecoveryCodeList recoveryCodeList;
        Gtk::Label notesLabel;
        Gtk::ScrolledWindow notesScroll;
        Gtk::TextView notesView;

        UniqueSignalTimeoutDispatcher previewTimeout;
        UniqueSignalTimeoutDispatcher clipboardClearTimeout;
    };
}
