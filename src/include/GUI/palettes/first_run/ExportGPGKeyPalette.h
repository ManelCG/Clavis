#pragma once

#include <gtkmm.h>

#include <GUI/palettes/DualChoicePalette.h>

namespace Clavis::GUI {
    class ExportGPGKeyPalette : public DualChoicePalette<ExportGPGKeyPalette> {
    public:
        ExportGPGKeyPalette();

        bool GetDoExportPrivate();
        std::filesystem::path GetExportPath();

    protected:

    private:
        void DoGiveResponse(bool r) override;

        std::filesystem::path exportPath;
        Gtk::CheckButton doExportPrivateCheckButton;
    };
}