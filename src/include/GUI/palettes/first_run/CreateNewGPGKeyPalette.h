#pragma once

#include <gtkmm.h>

#include <GUI/palettes/DualChoicePalette.h>

#include <GUI/components/ToggleIconButton.h>
#include <GUI/components/RequiredEntry.h>

#include <extensions/GPGWrapper.h>

namespace Clavis::GUI {
    class CreateNewGPGKeyPalette : public DualChoicePalette<CreateNewGPGKeyPalette> {
    public:
        CreateNewGPGKeyPalette();

        GPG::Key GetKey();

    protected:

    private:
        void SetTexts();
        void AppendWidgets();
        void SetStyles();

        void ApplyDisplayPasswordState();

        void DoGiveResponse(bool r) override;
        bool CheckBoxCorrectnessAndDisplayErrors();

        void PopulateKeytypes();
        void UpdateKeylength();

        GPG::KeyType GetSelectedKeyType();

        Gtk::Label mainLabel;

        Gtk::Label personalInfoLabel;

        Gtk::Box personalInfoHBox;
        RequiredEntry usernameEntry;
        RequiredEntry keynameEntry;
        Gtk::Entry commentEntry;

        Gtk::Label keyPasswordLabel;
        Gtk::Box passwordHBox;
        RequiredEntry passwordEntry;
        RequiredEntry repeatPasswordEntry;
        ToggleIconButton displayPasswordIconButton;

        Gtk::CheckButton advancedOptionsCheckButton;

        Gtk::Box advancedOptionsHBox;
        Gtk::Box advancedOptionsLabelsVBox;
        Gtk::Box advancedOptionsEntriesVBox;
        Gtk::Label keyTypeLabel;
        Gtk::ComboBoxText keyTypeEntry;
        Gtk::Label keyLengthLabel;
        RequiredEntry keyLengthEntry;

        std::pair<int, int> keySizeRange;
    };
}