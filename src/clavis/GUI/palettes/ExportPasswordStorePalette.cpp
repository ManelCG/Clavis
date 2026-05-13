#include <GUI/palettes/ExportPasswordStorePalette.h>

#include <GUI/workflows/NewItemWorkflow.h>
#include <language/Language.h>

namespace Clavis::GUI {
    ExportPasswordStorePalette::ExportPasswordStorePalette(const std::string& contextPath, const std::string& defaultFilename_)
        : DualChoicePalette(_(EXPORT_PASSWORD_STORE_PALETTE_TITLE))
        , defaultFilename(defaultFilename_)
    {
        SetYesText(_(MISC_EXPORT_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));
        SetYesSuggested();
        set_margin(10);

        // Context row (only shown when a specific folder is being exported)
        if (!contextPath.empty()) {
            contextInfoKeyLabel.set_markup("<b>" + std::string(_(EXPORT_PASSWORD_STORE_PALETTE_FOLDER_LABEL)) + "</b>");
            contextInfoKeyLabel.set_halign(Gtk::Align::START);
            contextInfoValueLabel.set_text(contextPath);
            contextInfoValueLabel.set_halign(Gtk::Align::START);
            contextInfoValueLabel.set_margin_start(5);
            contextInfoValueLabel.set_hexpand(true);
            contextInfoRow.append(contextInfoKeyLabel);
            contextInfoRow.append(contextInfoValueLabel);
            contextInfoRow.set_margin_bottom(8);
            sepContext.set_margin_bottom(8);
            append(contextInfoRow);
            append(sepContext);
        }

        // Path row
        pathEntry.set_placeholder_text(_(EXPORT_PASSWORD_STORE_PALETTE_PATH_PLACEHOLDER));
        pathEntry.set_text(defaultFilename);
        pathEntry.set_hexpand(true);
        browseButton.set_label(_(MISC_BROWSE_BUTTON));
        browseButton.set_margin_start(5);
        browseButton.signal_clicked().connect([this]() {
            std::string path = pathEntry.get_text();
            if (path.empty())
                path = defaultFilename;
            if (Workflows::OpenFileDialog(FileOpenDialogAction::SAVE_FILE, path, this))
                pathEntry.set_text(path);
        });
        pathRow.append(pathEntry);
        pathRow.append(browseButton);
        pathRow.set_margin_bottom(8);

        // Separator
        sep1.set_margin_bottom(8);

        // Encryption label
        encLabel.set_markup("<b>" + std::string(_(EXPORT_PASSWORD_STORE_PALETTE_ENCRYPTION_LABEL)) + "</b>");
        encLabel.set_halign(Gtk::Align::START);
        encLabel.set_margin_bottom(4);

        // Radio buttons
        radioNone.set_label(_(EXPORT_PASSWORD_STORE_PALETTE_NO_ENCRYPTION));
        radioPassword.set_label(_(EXPORT_PASSWORD_STORE_PALETTE_ENCRYPT_PASSWORD));
        radioGPGKey.set_label(_(EXPORT_PASSWORD_STORE_PALETTE_ENCRYPT_GPG_KEY));

        radioNone.set_active(true);
        radioPassword.set_group(radioNone);
        radioGPGKey.set_group(radioNone);

        radioNone.signal_toggled().connect([this]() { UpdatePasswordRowVisibility(); });
        radioPassword.signal_toggled().connect([this]() { UpdatePasswordRowVisibility(); });
        radioGPGKey.signal_toggled().connect([this]() { UpdatePasswordRowVisibility(); });

        radioGPGKey.set_margin_bottom(8);

        // Separator
        sep2.set_margin_bottom(8);

        // Password row
        passwordEntry.set_visibility(false);
        passwordEntry.set_input_purpose(Gtk::InputPurpose::PASSWORD);
        passwordEntry.set_placeholder_text(_(EXPORT_PASSWORD_STORE_PALETTE_PASSWORD_PLACEHOLDER));
        passwordEntry.set_hexpand(true);

        showPasswordButton.set_label(_(EXPORT_PASSWORD_STORE_PALETTE_SHOW_PASSWORD));
        showPasswordButton.set_margin_start(5);
        showPasswordButton.signal_clicked().connect([this]() {
            passwordVisible = !passwordVisible;
            passwordEntry.set_visibility(passwordVisible);
            showPasswordButton.set_label(
                passwordVisible
                    ? std::string(_(EXPORT_PASSWORD_STORE_PALETTE_HIDE_PASSWORD))
                    : std::string(_(EXPORT_PASSWORD_STORE_PALETTE_SHOW_PASSWORD))
            );
        });

        passwordRow.append(passwordEntry);
        passwordRow.append(showPasswordButton);

        // Assemble
        append(pathRow);
        append(sep1);
        append(encLabel);
        append(radioNone);
        append(radioPassword);
        append(radioGPGKey);
        append(sep2);
        append(passwordRow);

        UpdatePasswordRowVisibility();
    }

    void ExportPasswordStorePalette::UpdatePasswordRowVisibility() {
        bool showPassword = radioPassword.get_active();
        passwordRow.set_visible(showPassword);
        sep2.set_visible(showPassword);
    }

    std::filesystem::path ExportPasswordStorePalette::GetExportPath() const {
        return exportPath;
    }

    std::string ExportPasswordStorePalette::GetPassword() const {
        return passwordEntry.get_text();
    }

    Clav::EncryptionType ExportPasswordStorePalette::GetEncryptionMode() const {
        if (radioPassword.get_active()) return Clav::EncryptionType::Password;
        if (radioGPGKey.get_active())   return Clav::EncryptionType::GPGKey;
        return Clav::EncryptionType::None;
    }

    void ExportPasswordStorePalette::DoGiveResponse(bool r) {
        if (!r) {
            DualChoicePalette::DoGiveResponse(false);
            return;
        }

        auto path = std::string(pathEntry.get_text());
        if (path.empty()) {
            RaiseClavisError(_(ERROR_EXPORT_PATH_EMPTY));
            return;
        }

        // Ensure .clav extension
        if (path.size() < 5 || path.substr(path.size() - 5) != ".clav")
            path += ".clav";

        exportPath = std::filesystem::path(path);
        DualChoicePalette::DoGiveResponse(true);
    }
}
