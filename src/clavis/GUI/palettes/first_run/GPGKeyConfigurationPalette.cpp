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
            PopulateKeysComboBox(gpgKeysComboBox.get_active_id());
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
            std::string fingerprint;
            if (Workflows::ImportGPGWorkflow(this, fingerprint))
                PopulateKeysComboBox(fingerprint);
        });
        exportKeyButton.signal_clicked().connect([this]() {
            if (Workflows::ExportGPGWorkflow(GetGPGID()))
                PopulateKeysComboBox(gpgKeysComboBox.get_active_id());
        });
        createKeyButton.signal_clicked().connect([this]() {
            std::string fingerprint;
            if (Workflows::CreateGPGWorkflow(this, fingerprint))
                PopulateKeysComboBox(fingerprint);
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

    bool GPGKeyConfigurationPalette::TryPreselectGPGKey(const std::string &fingerprint) {
        auto currentID = gpgKeysComboBox.get_active_id();
        if (gpgKeysComboBox.set_active_id(fingerprint))
            return true;

        // Couldn't set the selected ID so we select the one we know works.
        gpgKeysComboBox.set_active_id(currentID);
        return false;
    }


    void GPGKeyConfigurationPalette::PopulateKeysComboBox(const std::string& fingerprint) {
        gpgKeysComboBox.remove_all();

        gpgKeys = GPG::GetAllKeys();

        bool fingerprintIsValid = false;
        std::string selectedFingerprint = fingerprint;
        std::string firstFingerprint;

        for (int i = 0; i < gpgKeys.size(); i++) {
            auto key = gpgKeys[i];
            auto str = GPG::KeyToString(key);
            auto id = key.fingerprint;

            // Prioritize the fingerprint we were told to show
            if (selectedFingerprint.empty())
                selectedFingerprint = id;
            // But it might be that it got deleted or something, and we are 100% sure that this fingerprint is valid
            if (firstFingerprint.empty())
                firstFingerprint = id;
            if (id == selectedFingerprint)
                fingerprintIsValid = true;

            gpgKeysComboBox.append(id, str);
        }

        if (!gpgKeys.empty()) {
            if (!selectedFingerprint.empty() && fingerprintIsValid)
                gpgKeysComboBox.set_active_id(selectedFingerprint);
            else
                gpgKeysComboBox.set_active_id(firstFingerprint);
        }

    }



}