#include <iostream>
#include <GUI/palettes/first_run/CreateNewGPGKeyPalette.h>

#include <language/Language.h>

namespace Clavis::GUI {
    CreateNewGPGKeyPalette::CreateNewGPGKeyPalette() :
        DualChoicePalette(_(CREATE_NEW_KEY_PALETTE_TITLE))
    {
        SetTexts();
        SetStyles();
        AppendWidgets();

        displayPasswordIconButton.signal_clicked().connect([this]() {
            ApplyDisplayPasswordState();
        });

        advancedOptionsCheckButton.signal_toggled().connect([this]() {
            auto state = advancedOptionsCheckButton.get_active();
            advancedOptionsHBox.set_visible(state);
        });

        styleDispatcher.connect([this]() {
            passwordEntry.remove_css_class("error");
            repeatPasswordEntry.remove_css_class("error");
            keyLengthEntry.remove_css_class("error");
        });

        keyTypeEntry.signal_changed().connect([this]() {
            UpdateKeylength();
        });

        ApplyDisplayPasswordState();
        advancedOptionsHBox.hide();

        PopulateKeytypes();

    }
    void CreateNewGPGKeyPalette::SetTexts() {
        SetYesText(_(DO_CREATE_KEY_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));

        mainLabel.set_text(_(CREATE_NEW_KEY_PALETTE_MAIN_LABEL));

        personalInfoLabel.set_text(_(CREATE_NEW_KEY_PALETTE_PERSONAL_INFORMATION_LABEL));
        usernameEntry.set_placeholder_text(_(CREATE_NEW_KEY_PALETTE_USERNAME_PLACEHOLDER));
        keynameEntry.set_placeholder_text(_(CREATE_NEW_KEY_PALETTE_KEY_NAME_PLACEHOLDER));
        commentEntry.set_placeholder_text(_(CREATE_NEW_KEY_PALETTE_COMMENT_PLACEHOLDER));

        keyPasswordLabel.set_text(_(CREATE_NEW_KEY_SET_KEY_PASSWORD_LABEL));
        passwordEntry.set_placeholder_text(_(CREATE_NEW_KEY_PASSWORD_PLACEHOLDER));
        repeatPasswordEntry.set_placeholder_text(_(CREATE_NEW_KEY_REPEAT_PASSWORD_PLACEHOLDER));
        displayPasswordIconButton.SetIconEnabled(Icons::Actions::Visible);
        displayPasswordIconButton.SetIconDisabled(Icons::Actions::Hidden);
        displayPasswordIconButton.SetState(false);

        advancedOptionsCheckButton.set_label(_(CREATE_NEW_KEY_ADVANCED_OPTIONS_LABEL));

        keyTypeLabel.set_text(_(CREATE_NEW_KEY_KEY_TYPE_LABEL));
        keyLengthLabel.set_text(_(CREATE_NEW_KEY_KEY_LENGTH_LABEL));
    }

    void CreateNewGPGKeyPalette::SetStyles() {
        set_margin(10);

        mainLabel.set_margin_bottom(20);
        personalInfoLabel.set_margin_bottom(5);
        personalInfoHBox.set_margin_bottom(5);
        usernameEntry.set_margin_end(5);
        commentEntry.set_margin_bottom(10);
        personalInfoHBox.set_hexpand(true);
        usernameEntry.set_hexpand(true);
        keynameEntry.set_hexpand(true);
        commentEntry.set_hexpand(true);

        keyPasswordLabel.set_margin_top(10);
        keyPasswordLabel.set_margin_bottom(5);
        passwordHBox.set_margin_bottom(10);
        passwordHBox.set_hexpand(true);
        passwordEntry.set_hexpand(true);
        repeatPasswordEntry.set_hexpand(true);
        passwordEntry.set_margin_end(5);
        repeatPasswordEntry.set_margin_end(5);

        advancedOptionsCheckButton.set_margin_top(10);
        advancedOptionsCheckButton.set_halign(Gtk::Align::CENTER);

        advancedOptionsHBox.set_hexpand(true);
        advancedOptionsHBox.set_margin_top(10);
        advancedOptionsHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        keyTypeLabel.set_hexpand(true);
        keyTypeEntry.set_hexpand(true);
        keyLengthLabel.set_hexpand(true);
        keyLengthEntry.set_hexpand(true);

        advancedOptionsLabelsVBox.set_orientation(Gtk::Orientation::VERTICAL);
        advancedOptionsLabelsVBox.set_vexpand(true);

        advancedOptionsEntriesVBox.set_orientation(Gtk::Orientation::VERTICAL);
        advancedOptionsEntriesVBox.set_vexpand(true);
        advancedOptionsEntriesVBox.set_valign(Gtk::Align::CENTER);

        keyTypeLabel.set_vexpand(true);
        keyLengthLabel.set_vexpand(true);
        keyTypeEntry.set_margin_bottom(5);
        keyTypeEntry.set_valign(Gtk::Align::CENTER);
        keyLengthEntry.set_valign(Gtk::Align::CENTER);

    }


    void CreateNewGPGKeyPalette::AppendWidgets() {
        append(mainLabel);

        append(personalInfoLabel);
        personalInfoHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        personalInfoHBox.append(usernameEntry);
        personalInfoHBox.append(keynameEntry);
        append(personalInfoHBox);
        append(commentEntry);

        auto sep1 = Gtk::Separator(Gtk::Orientation::HORIZONTAL);
        append(sep1);

        append(keyPasswordLabel);
        passwordHBox.append(passwordEntry);
        passwordHBox.append(repeatPasswordEntry);
        passwordHBox.append(displayPasswordIconButton);
        append(passwordHBox);

        auto sep2 = Gtk::Separator(Gtk::Orientation::HORIZONTAL);
        append(sep2);

        append(advancedOptionsCheckButton);
        advancedOptionsHBox.append(advancedOptionsLabelsVBox);
        advancedOptionsHBox.append(advancedOptionsEntriesVBox);

        advancedOptionsLabelsVBox.append(keyTypeLabel);
        advancedOptionsLabelsVBox.append(keyLengthLabel);

        advancedOptionsEntriesVBox.append(keyTypeEntry);
        advancedOptionsEntriesVBox.append(keyLengthEntry);

        append(advancedOptionsHBox);
    }

    void CreateNewGPGKeyPalette::ApplyDisplayPasswordState() {
        auto state = displayPasswordIconButton.GetState();
        passwordEntry.set_visibility(state);
        repeatPasswordEntry.set_visibility(state);
    }

    void CreateNewGPGKeyPalette::DoGiveResponse(bool r) {
        if (!r) {
            DualChoicePalette::DoGiveResponse(false);
            return;
        }

        bool success = true;

        if (!ArePasswordsValid()) {
            DisplayPasswordsError();
            success = false;;
        }

        if (!IsKeySizeValid()) {
            DisplayKeySizeError();
            success = false;
        }

        if (success)
            DualChoicePalette::DoGiveResponse(true);
    }

    bool CreateNewGPGKeyPalette::ArePasswordsValid() {
        auto p1 = passwordEntry.get_text();
        auto p2 = repeatPasswordEntry.get_text();

        if (p1 != p2)
            return false;

        if (p1.size() <= 3)
            return false;

        return true;
    }

    bool CreateNewGPGKeyPalette::IsKeySizeValid() {
        // keySize not available so always valid
        if (keySizeRange.first == -1 && keySizeRange.second == -1)
            return true;

        auto text = keyLengthEntry.get_text();
        int size = -1;
        try {
            size = std::stoi(text);
        } catch (...) { return false; }

        if (size >= keySizeRange.first && size <= keySizeRange.second)
            return true;

        return false;
    }


    void CreateNewGPGKeyPalette::DisplayPasswordsError() {
        passwordEntry.add_css_class("error");
        repeatPasswordEntry.add_css_class("error");

        styleSignalTimeout.ConnectOnce([this]() {
            styleDispatcher.emit();
        }, 3000);
    }
    void CreateNewGPGKeyPalette::DisplayKeySizeError() {
        keyLengthEntry.add_css_class("error");

        styleSignalTimeout.ConnectOnce([this]() {
            styleDispatcher.emit();
        }, 3000);
    }


    void CreateNewGPGKeyPalette::PopulateKeytypes() {
        auto types = GPG::GetAllKeyTypes();

        for (const auto& type : types) {
            auto code = GPG::KeyTypeToStringCode(type);
            auto name = GPG::KeyTypeToString(type);

            keyTypeEntry.append(code, name);
        }

        keyTypeEntry.set_active_id(GPG::KeyTypeToStringCode(GPG::KeyType::ECC_25519));
    }

    void CreateNewGPGKeyPalette::UpdateKeylength() {
        auto selectedType = GetSelectedKeyType();
        auto range = GPG::GetKeySizeRange(selectedType);

        // ECC. No key size available
        keySizeRange = {range.min, range.max};

        if (range.def == -1) {
            keyLengthEntry.set_visible(false);
            keyLengthLabel.set_visible(false);
            keyLengthEntry.set_text("");
        } else {
            keyLengthEntry.set_visible(true);
            keyLengthLabel.set_visible(true);
            keyLengthEntry.set_text(std::to_string(range.def));
            keyLengthEntry.set_placeholder_text(std::to_string(range.min) + " - " + std::to_string(range.max));
        }
    }

    GPG::KeyType CreateNewGPGKeyPalette::GetSelectedKeyType() {
        return GPG::StringCodeToKeyType(keyTypeEntry.get_active_id());
    }
}