#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <gtkmm.h>

#include <two_factor/MigrationImport.h>
#include <two_factor/TwoFactorEntry.h>

#include <GUI/components/IconButton.h>
#include <GUI/components/RequiredEntry.h>
#include <GUI/palettes/DualChoicePalette.h>

namespace Clavis::GUI {
    // Bulk import from another authenticator's otpauth-migration:// export. Every decoded
    // account is listed with a checkbox and an editable target name.
    class ImportTwoFactorMigrationPalette : public DualChoicePalette<ImportTwoFactorMigrationPalette> {
    public:
        // A non-empty initialUri (pasted into the New 2FA dialog) is parsed immediately.
        explicit ImportTwoFactorMigrationPalette(const std::string& initialUri);

        [[nodiscard]] std::vector<std::pair<std::string, TwoFactor::TwoFactorEntry>> GetSelectedEntries() const;

    protected:
        void DoGiveResponse(bool r) override;
        [[nodiscard]] bool ShouldSubmitOnReturn() const override;

    private:
        struct Row {
            Gtk::Box* box;
            Gtk::CheckButton* check;
            RequiredEntry* nameEntry;
            TwoFactor::TwoFactorEntry entry;
        };

        void ParseUri();
        void ClearRows();

        TwoFactor::MigrationResult result;
        std::vector<Row> rows;
        std::vector<std::unique_ptr<Gtk::Widget>> ownedWidgets;

        Gtk::Box mainVBox;
        Gtk::Label instructionsLabel;
        Gtk::Box uriHBox;
        RequiredEntry uriEntry;
        IconButton parseButton;
        Gtk::Label summaryLabel;
        Gtk::ScrolledWindow rowsScroll;
        Gtk::Box rowsVBox;
    };
}
