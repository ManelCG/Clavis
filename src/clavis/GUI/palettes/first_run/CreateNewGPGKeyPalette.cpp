#include <gpgme.h>
#include <iostream>
#include <GUI/palettes/first_run/CreateNewGPGKeyPalette.h>

#include <language/Language.h>

#include <error/ClavisError.h>

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

        keyTypeEntry.signal_changed().connect([this]() {
            UpdateKeylength();
        });

        passwordEntry.SetCorrectnessCheck([this]() {
            return passwordEntry.get_text().size() > 3;
        });
        repeatPasswordEntry.SetCorrectnessCheck([this]() {
            return passwordEntry.get_text() == repeatPasswordEntry.get_text();
        });
        keyLengthEntry.SetCorrectnessCheck([this]() {
            // keySize not available so always valid
            if (keySizeRange.first == -1 && keySizeRange.second == -1)
                return true;

            const auto text = keyLengthEntry.get_text();
            int size = -1;
            try {
                size = std::stoi(text);
            } catch (...) { return false; }

            if (size >= keySizeRange.first && size <= keySizeRange.second)
                return true;

            return false;
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

    bool CreateNewGPGKeyPalette::CheckBoxCorrectnessAndDisplayErrors() {
        bool success = true;

        if (!usernameEntry.IsValid()) {
            usernameEntry.DisplayError();
            success = false;
        }

        if (!keynameEntry.IsValid()) {
            keynameEntry.DisplayError();
            success = false;
        }

        if (!passwordEntry.IsValid()) {
            passwordEntry.DisplayError();
            repeatPasswordEntry.DisplayError();
            success = false;;
        }

        if (!repeatPasswordEntry.IsValid()) {
            repeatPasswordEntry.DisplayError();
            success = false;
        }

        if (!keyLengthEntry.IsValid()) {
            keyLengthEntry.DisplayError();
            success = false;
        }

        return success;
    }


    void CreateNewGPGKeyPalette::DoGiveResponse(bool r) {
        if (!r) {
            DualChoicePalette::DoGiveResponse(false);
            return;
        }

        auto success = CheckBoxCorrectnessAndDisplayErrors();

        if (success)
            DualChoicePalette::DoGiveResponse(true);
    }


    void CreateNewGPGKeyPalette::PopulateKeytypes() {
        auto types = GPG::GetAllKeyTypes();

        for (const auto& type : types) {
            auto code = GPG::KeyTypeToStringCode(type);
            auto name = GPG::KeyTypeToString(type);

            keyTypeEntry.append(code, name);
        }

        keyTypeEntry.set_active_id(GPG::KeyTypeToStringCode(GPG::KeyType::ECC_25519));

        UpdateKeylength();
    }

    void CreateNewGPGKeyPalette::UpdateKeylength() {
        auto selectedType = GetSelectedKeyType();
        auto range = GPG::GetKeySizeRange(selectedType);

        // ECC. No key size available
        keySizeRange = {range.min, range.max};

        if (range.def == -1) {
            keyLengthEntry.set_visible(false);
            keyLengthLabel.set_visible(false);
            keyLengthEntry.set_text("-1");
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

    GPG::Key CreateNewGPGKeyPalette::GetKey() {
        if (!CheckBoxCorrectnessAndDisplayErrors())
            RaiseClavisError(_(ERROR_GPG_KEY_IS_NOT_VALID));
        GPG::Key key;

        key.username = usernameEntry.get_text();
        key.keyname = keynameEntry.get_text();
        key.comment = commentEntry.get_text();

        key.type = GetSelectedKeyType();
        key.length = std::stoi(keyLengthEntry.get_text());

        key.password = passwordEntry.get_text();

        return key;
    }

}