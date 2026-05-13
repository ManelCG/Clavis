#pragma once

#include <filesystem>
#include <string>

#include <gtkmm.h>

#include <GUI/palettes/DualChoicePalette.h>
#include <clav/ClavFile.h>

namespace Clavis::GUI {
    class ImportPasswordStorePalette : public DualChoicePalette<ImportPasswordStorePalette> {
    public:
        ImportPasswordStorePalette();

        std::filesystem::path GetFilePath() const;
        std::string           GetPassword()  const;

    protected:
        void DoGiveResponse(bool r) override;

    private:
        void ValidatePath();
        void SetStatus(bool isError, const std::string& message);
        void ClearStatus();
        void SetPasswordRowVisible(bool visible);
        void OnPasswordChanged();

        Gtk::Box    pathRow{Gtk::Orientation::HORIZONTAL};
        Gtk::Entry  pathEntry;
        Gtk::Button browseButton;

        Gtk::Label statusLabel;

        Gtk::Separator passwordSep;
        Gtk::Box    passwordRow{Gtk::Orientation::HORIZONTAL};
        Gtk::Entry  passwordEntry;
        Gtk::Button showPasswordButton;
        bool        passwordVisible = false;

        std::filesystem::path filePath;
        bool isPasswordMode = false;
    };
}
