#pragma once

#include <gtkmm.h>

#include <GUI/palettes/DualChoicePalette.h>

#include <GUI/components/ToggleIconButton.h>
#include <GUI/signals/UniqueSignalTimeout.h>

#include <extensions/GPGWrapper.h>

namespace Clavis::GUI {
    class CreateNewGPGKeyPalette : public DualChoicePalette<CreateNewGPGKeyPalette> {
    public:
        CreateNewGPGKeyPalette();

    protected:

    private:
        void SetTexts();
        void AppendWidgets();
        void SetStyles();

        void ApplyDisplayPasswordState();

        bool ArePasswordsValid();
        bool IsKeySizeValid();
        void DoGiveResponse(bool r) override;

        void DisplayPasswordsError();
        void DisplayKeySizeError();

        void PopulateKeytypes();
        void UpdateKeylength();

        GPG::KeyType GetSelectedKeyType();

        Gtk::Label mainLabel;

        Gtk::Label personalInfoLabel;

        Gtk::Box personalInfoHBox;
        Gtk::Entry usernameEntry;
        Gtk::Entry keynameEntry;
        Gtk::Entry commentEntry;

        Gtk::Label keyPasswordLabel;
        Gtk::Box passwordHBox;
        Gtk::Entry passwordEntry;
        Gtk::Entry repeatPasswordEntry;
        ToggleIconButton displayPasswordIconButton;

        Gtk::CheckButton advancedOptionsCheckButton;

        Gtk::Box advancedOptionsHBox;
        Gtk::Box advancedOptionsLabelsVBox;
        Gtk::Box advancedOptionsEntriesVBox;
        Gtk::Label keyTypeLabel;
        Gtk::ComboBoxText keyTypeEntry;
        Gtk::Label keyLengthLabel;
        Gtk::Entry keyLengthEntry;

        std::pair<int, int> keySizeRange;
        Glib::Dispatcher styleDispatcher;
        UniqueSignalTimeout styleSignalTimeout;
    };
}