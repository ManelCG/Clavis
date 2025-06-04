#include <GUI/palettes/first_run/ExportGPGKeyPalette.h>

#include <GUI/workflows/NewItemWorkflow.h>

namespace Clavis::GUI {
    ExportGPGKeyPalette::ExportGPGKeyPalette() :
        DualChoicePalette(_(EXPORT_GPG_KEY_TITLE))
    {
        SetYesText(_(MISC_EXPORT_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));

        doExportPrivateCheckButton.set_label(_(DO_EXPORT_PRIVATE_KEY_CHECKBUTTON));

        append(doExportPrivateCheckButton);
        doExportPrivateCheckButton.set_margin(10);
    }

    bool ExportGPGKeyPalette::GetDoExportPrivate() {
        return doExportPrivateCheckButton.get_active();
    }

    std::filesystem::path ExportGPGKeyPalette::GetExportPath() {
        return exportPath;
    }

    void ExportGPGKeyPalette::DoGiveResponse(bool r) {
        if (!r) {
            DualChoicePalette::DoGiveResponse(false);
            return;
        }

        std::string path;
        if (!Workflows::OpenFileDialog(FileOpenDialogAction::SAVE_FILE, path, this))
            return;

        exportPath = std::filesystem::path(path);
        DualChoicePalette::DoGiveResponse(true);
    }

}