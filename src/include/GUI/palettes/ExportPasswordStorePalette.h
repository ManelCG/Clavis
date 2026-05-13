#pragma once

#include <filesystem>
#include <string>

#include <gtkmm.h>

#include <GUI/palettes/DualChoicePalette.h>
#include <clav/ClavFile.h>

namespace Clavis::GUI {
    class ExportPasswordStorePalette : public DualChoicePalette<ExportPasswordStorePalette> {
    public:
        explicit ExportPasswordStorePalette(
            const std::string& contextPath    = "",
            const std::string& defaultFilename = "export.clav"
        );

        std::filesystem::path     GetExportPath()     const;
        std::string               GetPassword()       const;
        Clav::EncryptionType      GetEncryptionMode() const;

    protected:
        void DoGiveResponse(bool r) override;

    private:
        void UpdatePasswordRowVisibility();

        std::string defaultFilename;

        Gtk::Box   contextInfoRow{Gtk::Orientation::HORIZONTAL};
        Gtk::Label contextInfoKeyLabel;
        Gtk::Label contextInfoValueLabel;
        Gtk::Separator sepContext;

        Gtk::Box pathRow{Gtk::Orientation::HORIZONTAL};
        Gtk::Entry pathEntry;
        Gtk::Button browseButton;

        Gtk::Separator sep1, sep2;

        Gtk::Label encLabel;
        Gtk::CheckButton radioNone;
        Gtk::CheckButton radioPassword;
        Gtk::CheckButton radioGPGKey;

        Gtk::Box passwordRow{Gtk::Orientation::HORIZONTAL};
        Gtk::Entry passwordEntry;
        Gtk::Button showPasswordButton;
        bool passwordVisible = false;

        std::filesystem::path exportPath;
    };
}
