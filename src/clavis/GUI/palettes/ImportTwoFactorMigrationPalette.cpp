#include <GUI/palettes/ImportTwoFactorMigrationPalette.h>

#include <extensions/StringHelper.h>
#include <language/Language.h>

namespace Clavis::GUI {
    ImportTwoFactorMigrationPalette::ImportTwoFactorMigrationPalette(const std::string& initialUri)
        : DualChoicePalette(_(IMPORT_TWO_FACTOR_PALETTE_TITLE)),
          mainVBox(Gtk::Orientation::VERTICAL),
          uriHBox(Gtk::Orientation::HORIZONTAL),
          rowsVBox(Gtk::Orientation::VERTICAL) {
        set_default_size(560, -1);
        SetYesSuggested();

        instructionsLabel.set_text(_(IMPORT_TWO_FACTOR_PALETTE_LABEL));
        instructionsLabel.set_wrap(true);
        instructionsLabel.set_halign(Gtk::Align::START);
        instructionsLabel.set_margin_bottom(5);
        mainVBox.append(instructionsLabel);

        uriEntry.set_placeholder_text("otpauth-migration://offline?data=...");
        uriEntry.set_hexpand(true);
        parseButton.SetIcon(Icons::Actions::Import);
        parseButton.set_tooltip_text(_(IMPORT_TWO_FACTOR_PALETTE_PARSE_BUTTON));
        parseButton.set_margin_start(5);
        uriHBox.append(uriEntry);
        uriHBox.append(parseButton);
        mainVBox.append(uriHBox);

        summaryLabel.set_halign(Gtk::Align::START);
        summaryLabel.set_wrap(true);
        summaryLabel.set_margin_top(10);
        mainVBox.append(summaryLabel);

        rowsScroll.set_child(rowsVBox);
        rowsScroll.set_size_request(-1, 220);
        rowsScroll.set_margin_top(5);
        rowsScroll.set_has_frame(true);
        mainVBox.append(rowsScroll);

        append(mainVBox);

        parseButton.signal_clicked().connect([this]() {
            ParseUri();
        });
        uriEntry.signal_activate().connect([this]() {
            ParseUri();
        });

        if (!initialUri.empty()) {
            uriEntry.set_text(initialUri);
            ParseUri();
        }
    }

    void ImportTwoFactorMigrationPalette::ClearRows() {
        for (const auto& row : rows)
            rowsVBox.remove(*row.box);

        rows.clear();
        ownedWidgets.clear();
    }

    void ImportTwoFactorMigrationPalette::ParseUri() {
        ClearRows();

        const auto uri = StringHelper::Trim(uriEntry.get_text());
        if (uri.empty())
            return;

        if (!TwoFactor::TryParseMigrationUri(uri, result)) {
            uriEntry.DisplayError();
            summaryLabel.set_text(_(ERROR_MIGRATION_PARSE_FAILED));
            return;
        }

        uriEntry.DisplaySuccess();

        auto summary = Glib::ustring(_(IMPORT_TWO_FACTOR_PALETTE_SUMMARY, std::to_string(result.entries.size())));
        if (!result.unsupportedAccounts.empty())
            summary += "\n" + Glib::ustring(_(IMPORT_TWO_FACTOR_PALETTE_UNSUPPORTED,
                                              std::to_string(result.unsupportedAccounts.size())));
        summaryLabel.set_text(summary);

        for (const auto& entry : result.entries) {
            auto box = std::make_unique<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
            auto check = std::make_unique<Gtk::CheckButton>();
            auto nameEntry = std::make_unique<RequiredEntry>();

            check->set_active(true);
            check->set_margin_end(5);

            nameEntry->set_text(entry.GetSuggestedName());
            nameEntry->set_hexpand(true);

            auto description = std::make_unique<Gtk::Label>(
                Glib::ustring(entry.GetType() == TwoFactor::OtpType::HOTP ? "HOTP" : "TOTP") + " / " +
                Glib::ustring(Crypto::HashAlgorithmToString(entry.GetAlgorithm())) + " / " +
                Glib::ustring(std::to_string(entry.GetDigits())));
            description->set_margin_start(8);
            description->add_css_class("dim-label");

            box->append(*check);
            box->append(*nameEntry);
            box->append(*description);
            box->set_margin(4);

            rowsVBox.append(*box);

            rows.push_back(Row{ box.get(), check.get(), nameEntry.get(), entry });

            ownedWidgets.push_back(std::move(check));
            ownedWidgets.push_back(std::move(nameEntry));
            ownedWidgets.push_back(std::move(description));
            ownedWidgets.push_back(std::move(box));
        }
    }

    std::vector<std::pair<std::string, TwoFactor::TwoFactorEntry>>
    ImportTwoFactorMigrationPalette::GetSelectedEntries() const {
        std::vector<std::pair<std::string, TwoFactor::TwoFactorEntry>> selected;

        for (const auto& row : rows) {
            if (!row.check->get_active())
                continue;

            selected.emplace_back(StringHelper::Trim(row.nameEntry->get_text()), row.entry);
        }

        return selected;
    }

    bool ImportTwoFactorMigrationPalette::ShouldSubmitOnReturn() const {
        // Return in the URI box reads the codes; it should not accept an empty dialog.
        const auto* focus = get_focus();

        // is_ancestor takes a non-const reference; the call itself does not mutate.
        return focus == nullptr || !focus->is_ancestor(const_cast<RequiredEntry&>(uriEntry));
    }

    void ImportTwoFactorMigrationPalette::DoGiveResponse(bool r) {
        if (!r) {
            DualChoicePalette::DoGiveResponse(false);
            return;
        }

        bool success = true;
        std::vector<std::string> seenNames;

        for (const auto& row : rows) {
            if (!row.check->get_active())
                continue;

            const auto name = StringHelper::Trim(row.nameEntry->get_text());

            // An empty or duplicated name would silently collapse two accounts into one file.
            if (name.empty() ||
                std::find(seenNames.begin(), seenNames.end(), name) != seenNames.end()) {
                success = false;
                row.nameEntry->DisplayError();
                continue;
            }

            seenNames.push_back(name);
        }

        if (seenNames.empty())
            success = false;

        if (!success)
            return;

        DualChoicePalette::DoGiveResponse(true);
    }
}
