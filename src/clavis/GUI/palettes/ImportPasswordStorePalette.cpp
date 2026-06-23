#include <GUI/palettes/ImportPasswordStorePalette.h>

#include <GUI/workflows/NewItemWorkflow.h>
#include <language/Language.h>
#include <system/Extensions.h>

namespace Clavis::GUI {

    ImportPasswordStorePalette::ImportPasswordStorePalette()
        : DualChoicePalette(_(IMPORT_PASSWORD_STORE_PALETTE_TITLE))
    {
        SetYesText(_(MISC_IMPORT_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));
        SetYesEnabled(false);
        set_margin(10);

        // Path row
        pathEntry.set_placeholder_text(_(IMPORT_PASSWORD_STORE_PALETTE_PATH_PLACEHOLDER));
        pathEntry.set_hexpand(true);
        browseButton.set_label(_(MISC_BROWSE_BUTTON));
        browseButton.set_margin_start(5);
        browseButton.signal_clicked().connect([this]() {
            std::string path = pathEntry.get_text();
            if (Workflows::OpenFileDialog(FileOpenDialogAction::OPEN_FILE, path, this))
                pathEntry.set_text(path);
        });
        pathRow.append(pathEntry);
        pathRow.append(browseButton);
        pathRow.set_margin_bottom(8);

        // Status label (hidden initially)
        statusLabel.set_halign(Gtk::Align::START);
        statusLabel.set_use_markup(true);
        statusLabel.set_visible(false);
        statusLabel.set_wrap(true);
        statusLabel.set_margin_bottom(8);

        // Password row (hidden initially)
        passwordEntry.set_visibility(false);
        passwordEntry.set_input_purpose(Gtk::InputPurpose::PASSWORD);
        passwordEntry.set_placeholder_text(_(IMPORT_PASSWORD_STORE_PALETTE_PASSWORD_PLACEHOLDER));
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
        passwordSep.set_margin_bottom(8);

        // Assemble
        append(pathRow);
        append(statusLabel);
        append(passwordSep);
        append(passwordRow);

        SetPasswordRowVisible(false);

        pathEntry.signal_changed().connect([this]() { ValidatePath(); });
        passwordEntry.signal_changed().connect([this]() { OnPasswordChanged(); });
    }

    void ImportPasswordStorePalette::SetPasswordRowVisible(bool visible) {
        passwordSep.set_visible(visible);
        passwordRow.set_visible(visible);
        isPasswordMode = visible;
    }

    void ImportPasswordStorePalette::SetStatus(bool isError, const std::string& message) {
        const std::string color = isError ? "red" : "green";
        statusLabel.set_markup("<span foreground='" + color + "'>" + message + "</span>");
        statusLabel.set_visible(true);
    }

    void ImportPasswordStorePalette::ClearStatus() {
        statusLabel.set_visible(false);
    }

    void ImportPasswordStorePalette::OnPasswordChanged() {
        if (!isPasswordMode) return;
        SetYesEnabled(!passwordEntry.get_text().empty());
    }

    void ImportPasswordStorePalette::ValidatePath() {
        const auto path = std::string(pathEntry.get_text());
        ClearStatus();
        SetPasswordRowVisible(false);
        SetYesEnabled(false);

        if (path.empty() || !System::FileExists(path))
            return;

        std::vector<uint8_t> fileData;
        if (!System::TryReadFile(path, fileData))
            return;

        Clav::EncryptionType enc;
        const auto fmtResult = Clav::ClavFile::TryCheckFormat(fileData, enc);

        if (fmtResult == Clav::ClavReadResult::NotAClavFile) {
            SetStatus(true, _(IMPORT_PASSWORD_STORE_PALETTE_STATUS_NOT_A_CLAV));
            return;
        }
        if (fmtResult == Clav::ClavReadResult::UnsupportedVersion) {
            SetStatus(true, _(IMPORT_PASSWORD_STORE_PALETTE_STATUS_UNSUPPORTED_VERSION));
            return;
        }

        if (enc == Clav::EncryptionType::Password) {
            SetStatus(false, _(IMPORT_PASSWORD_STORE_PALETTE_STATUS_PASSWORD_ENCRYPTED));
            SetPasswordRowVisible(true);
            SetYesEnabled(!passwordEntry.get_text().empty());
            return;
        }

        // None or GPGKey: try to read the full payload now
        Clav::ParsedClavFile parsed;
        const auto readResult = Clav::ClavFile::TryRead(fileData, parsed);

        if (readResult == Clav::ClavReadResult::DecryptionFailed) {
            SetStatus(true, _(IMPORT_PASSWORD_STORE_PALETTE_STATUS_CANNOT_DECRYPT));
            return;
        }
        if (readResult != Clav::ClavReadResult::Ok) {
            SetStatus(true, _(IMPORT_PASSWORD_STORE_PALETTE_STATUS_NOT_A_CLAV));
            return;
        }

        if (!parsed.publicKeyData.empty() && !Clav::ClavFile::CheckPublicKeyMatchesStore(parsed.publicKeyData)) {
            SetStatus(true, _(IMPORT_PASSWORD_STORE_PALETTE_STATUS_KEY_MISMATCH));
            return;
        }

        if (enc == Clav::EncryptionType::None)
            SetStatus(false, _(IMPORT_PASSWORD_STORE_PALETTE_STATUS_VALID_NO_ENCRYPTION));
        else
            SetStatus(false, _(IMPORT_PASSWORD_STORE_PALETTE_STATUS_VALID_GPG));

        SetYesEnabled(true);
    }

    std::filesystem::path ImportPasswordStorePalette::GetFilePath() const {
        return filePath;
    }

    std::string ImportPasswordStorePalette::GetPassword() const {
        return std::string(passwordEntry.get_text());
    }

    void ImportPasswordStorePalette::DoGiveResponse(bool r) {
        if (!r) {
            DualChoicePalette::DoGiveResponse(false);
            return;
        }

        if (isPasswordMode) {
            std::vector<uint8_t> fileData;
            if (!System::TryReadFile(std::string(pathEntry.get_text()), fileData)) {
                SetStatus(true, _(ERROR_IMPORT_FAILED));
                return;
            }

            Clav::ParsedClavFile parsed;
            const auto result = Clav::ClavFile::TryRead(fileData, parsed, std::string(passwordEntry.get_text()));

            if (result == Clav::ClavReadResult::DecryptionFailed) {
                SetStatus(true, _(ERROR_IMPORT_WRONG_PASSWORD));
                return;
            }
            if (result != Clav::ClavReadResult::Ok) {
                SetStatus(true, _(ERROR_IMPORT_FAILED));
                return;
            }

            if (!parsed.publicKeyData.empty() && !Clav::ClavFile::CheckPublicKeyMatchesStore(parsed.publicKeyData)) {
                SetStatus(true, _(IMPORT_PASSWORD_STORE_PALETTE_STATUS_KEY_MISMATCH));
                return;
            }
        }

        filePath = std::filesystem::path(std::string(pathEntry.get_text()));
        DualChoicePalette::DoGiveResponse(true);
    }

}
