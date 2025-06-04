#include <GUI/palettes/first_run/GPGKeyConfigurationPalette.h>

#include <GUI/palettes/first_run/CreateNewGPGKeyPalette.h>

#include <GUI/workflows/NewItemWorkflow.h>

namespace Clavis::GUI {
    GPGKeyConfigurationPalette::GPGKeyConfigurationPalette() :
        DualChoicePalette(_(FIRST_RUN_GPG_KEY_PALETTE_TITLE))
    {
        set_margin(10);
        set_default_size(420, -1);

        SetYesText(_(MISC_CONFIRM_SELECTION_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));

        PopulateKeysComboBox();


        mainLabel.set_text(_(FIRST_RUN_GPG_KEY_PALETTE_LABEL));


        refreshKeysButton.SetIcon(Icons::Actions::Refresh);
        refreshKeysButton.signal_clicked().connect([this]() {
            PopulateKeysComboBox();
        });

        keysHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        keysHBox.append(gpgKeysComboBox);
        keysHBox.append(refreshKeysButton);
        keysHBox.set_hexpand(true);
        gpgKeysComboBox.set_hexpand(true);
        gpgKeysComboBox.set_margin_end(5);
        keysHBox.set_margin_top(10);
        keysHBox.set_margin_bottom(5);

        importKeyButton.SetIcon(Icons::Actions::Import);
        exportKeyButton.SetIcon(Icons::Actions::Save);
        createKeyButton.SetIcon(Icons::Actions::NewDocument);

        importKeyButton.SetLabel(_(GPG_KEY_PALETTE_IMPORT_KEY_BUTTON));
        exportKeyButton.SetLabel(_(GPG_KEY_PALETTE_EXPORT_KEY_BUTTON));
        createKeyButton.SetLabel(_(GPG_KEY_PALETTE_CREATE_NEW_KEY_BUTTON));

        importKeyButton.signal_clicked().connect([this]() {
            if (Workflows::ImportGPGWorkflow(this))
                PopulateKeysComboBox();
        });
        exportKeyButton.signal_clicked().connect([this]() {
            if (Workflows::ExportGPGWorkflow(this))
                PopulateKeysComboBox();
        });
        createKeyButton.signal_clicked().connect([this]() {
            if (Workflows::CreateGPGWorkflow(this))
                PopulateKeysComboBox();
        });

        importExportCreateHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        importExportCreateHBox.append(importKeyButton);
        importExportCreateHBox.append(exportKeyButton);
        importExportCreateHBox.append(createKeyButton);
        importExportCreateHBox.set_hexpand(true);
        importKeyButton.set_hexpand(true);
        exportKeyButton.set_hexpand(true);
        createKeyButton.set_hexpand(true);
        importKeyButton.set_margin_end(5);
        exportKeyButton.set_margin_end(5);
        // importExportCreateHBox.set_margin_bottom(10);

        append(mainLabel);
        append(keysHBox);
        append(importExportCreateHBox);

        // auto sep = Gtk::Separator(Gtk::Orientation::HORIZONTAL);
        // append(sep);
    }

    std::string GPGKeyConfigurationPalette::GetGPGID() {
        return gpgKeysComboBox.get_active_text();
    }

    void GPGKeyConfigurationPalette::PopulateKeysComboBox() {
        gpgKeysComboBox.remove_all();

        gpgKeys = GPG::GetAllKeys();

        for (int i = 0; i < gpgKeys.size(); i++) {
            auto id = std::to_string(i);
            auto key = gpgKeys[i];
            auto str = GPG::KeyToString(key);

            gpgKeysComboBox.append(id, str);
        }

        if (gpgKeys.size() > 0)
            gpgKeysComboBox.set_active_id(std::to_string(0));
    }



}