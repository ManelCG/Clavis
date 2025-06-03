#pragma once

#include <filesystem>

#include <gtkmm.h>

#include <extensions/GPGWrapper.h>

#include <GUI/palettes/DualChoicePalette.h>
#include <GUI/components/IconButton.h>

namespace Clavis::GUI {
    class GPGKeyConfigurationPalette : public DualChoicePalette<GPGKeyConfigurationPalette> {
    public:
        GPGKeyConfigurationPalette();

        std::string GetGPGID();

    protected:

    private:
        Gtk::Label mainLabel;

        Gtk::Box keysHBox;
        Gtk::ComboBoxText gpgKeysComboBox;
        IconButton refreshKeysButton;

        Gtk::Box importExportCreateHBox;
        LabeledIconButton importKeyButton;
        LabeledIconButton exportKeyButton;
        LabeledIconButton createKeyButton;

        std::vector<GPG::Key> gpgKeys;

        void PopulateKeysComboBox();
    };
}