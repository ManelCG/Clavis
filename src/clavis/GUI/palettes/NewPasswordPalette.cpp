#include "GUI/palettes/NewPasswordPalette.h"

#include <GUI/palettes/NewPasswordPalette.h>

#include <language/Language.h>

#include "settings/SettingsDefinitions.h"

namespace Clavis::GUI {
#pragma region NEW PASSWORD WIDGET
    NewPasswordPalette::NewPasswordWidget::NewPasswordWidget() :
        Gtk::Box(Gtk::Orientation::VERTICAL),

        passwordEntryHBox(Gtk::Orientation::HORIZONTAL)
    {
        setPasswordNameLabel.set_text(_(NEW_PASSWORD_PALETTE_SET_PASSWORD_NAME_LABEL_TEXT));
        newPasswordLabel.set_text(_(NEW_PASSWORD_PALETTE_NEW_PASSWORD_LABEL_TEXT));
        nameEntry.set_text(_(NEW_PASSWORD_PALETTE_DEFAULT_PASSWORD_NAME));
        passwordEntry.set_placeholder_text(_(NEW_PASSWORD_PALETTE_PASSWORD_ENTRY_PLACEHOLDER));

        generatePasswordButton.SetIcon(Icons::Actions::Refresh);
        displayPasswordButton.SetIcons(Icons::Actions::Visible, Icons::Actions::Hidden);
        displayPasswordButton.SetState(true);

        passwordEntryHBox.append(passwordEntry);
        passwordEntryHBox.append(generatePasswordButton);
        passwordEntryHBox.append(displayPasswordButton);

        nameEntry.set_margin_top(10);
        newPasswordLabel.set_margin_top(35);
        passwordEntry.set_margin_end(5);
        displayPasswordButton.set_margin_end(5);
        passwordEntryHBox.set_margin_top(10);

        displayPasswordButton.signal_clicked().connect([this]() {
            passwordEntry.set_visibility(displayPasswordButton.GetState());
        });
        generatePasswordButton.signal_clicked().connect([this]() {
            passwordEntry.set_text(PasswordGenerator::GeneratePassword(settingsProvider()));
        });

        append(setPasswordNameLabel);
        append(nameEntry);
        append(newPasswordLabel);
        append(passwordEntryHBox);
    }

    void NewPasswordPalette::NewPasswordWidget::SetSettingsProvider(std::function<PasswordGenerator::GeneratorSettings()> lambda) {
        settingsProvider = lambda;
    }

    std::string NewPasswordPalette::NewPasswordWidget::GetPasswordName() const {
        return nameEntry.get_text();
    }

    std::string NewPasswordPalette::NewPasswordWidget::GetPassword() const {
        return passwordEntry.get_text();
    }

    void NewPasswordPalette::NewPasswordWidget::SetFixedName(const std::string &name) {
        nameEntry.set_text(name);
        nameEntry.set_sensitive(false);
        nameEntry.set_editable(false);

        setPasswordNameLabel.set_text(_(NEW_PASSWORD_PALETTE_CURRENT_PASSWORD_NAME_LABEL_TEXT));
    }


#pragma endregion


#pragma region PASSWORD GENERATOR CONTROLLER
    NewPasswordPalette::PasswordGeneratorController::PasswordGeneratorController(const PasswordGenerator::GeneratorSettings &settings) :
        Gtk::Box(Gtk::Orientation::VERTICAL),

        lengthHBox(Gtk::Orientation::HORIZONTAL),

        bigHBox(Gtk::Orientation::HORIZONTAL),
        leftVBox(Gtk::Orientation::VERTICAL),
        rightVBox(Gtk::Orientation::VERTICAL)
    {
        passwordGeneratorLabel.set_text(_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_LABEL_TEXT));

        lengthLabel.set_text(_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_LENGTH_LABEL_TEXT));
        lengthHBox.append(lengthLabel);
        lengthHBox.append(lengthEntry);
        lengthHBox.set_margin_top(10);
        lengthEntry.set_margin_start(5);

        lowercaseToggle.set_label(_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_LOWERCASE));
        uppercaseToggle.set_label(_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_UPPERCASE));
        numeralsToggle.set_label(_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_NUMERALS));
        symbolsToggle.set_label(_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_SYMBOLS));
        pronounceableToggle.set_label(_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_PRONOUNCEABLE));

        lengthEntry.signal_changed().connect([this](){ SaveCurrentSettings();});
        lowercaseToggle.signal_toggled().connect([this]() {SaveCurrentSettings();});
        uppercaseToggle.signal_toggled().connect([this](){SaveCurrentSettings();});
        numeralsToggle.signal_toggled().connect([this](){SaveCurrentSettings();});
        symbolsToggle.signal_toggled().connect([this](){SaveCurrentSettings();});
        pronounceableToggle.signal_toggled().connect([this](){SaveCurrentSettings();});

        leftVBox.append(lowercaseToggle);
        leftVBox.append(numeralsToggle);
        leftVBox.append(pronounceableToggle);
        rightVBox.append(uppercaseToggle);
        rightVBox.append(symbolsToggle);

        bigHBox.append(leftVBox);
        bigHBox.append(rightVBox);
        bigHBox.set_hexpand(true);
        leftVBox.set_hexpand(true);
        rightVBox.set_hexpand(true);
        rightVBox.set_halign(Gtk::Align::END);
        bigHBox.set_margin_top(20);

        append(passwordGeneratorLabel);
        append(lengthHBox);
        append(bigHBox);

        PopulateWithValues(settings);
    }


    void NewPasswordPalette::PasswordGeneratorController::PopulateWithValues(const PasswordGenerator::GeneratorSettings &settings) {
        lengthEntry.SetValue(settings.length);
        lowercaseToggle.set_active(settings.lowercase);
        uppercaseToggle.set_active(settings.uppercase);
        numeralsToggle.set_active(settings.numerals);
        symbolsToggle.set_active(settings.symbols);
        pronounceableToggle.set_active(settings.pronounceable);
    }

    PasswordGenerator::GeneratorSettings NewPasswordPalette::PasswordGeneratorController::GetSettings() {
        PasswordGenerator::GeneratorSettings settings;

        settings.length = lengthEntry.GetValue();
        settings.lowercase = lowercaseToggle.get_active();
        settings.uppercase = uppercaseToggle.get_active();
        settings.numerals = numeralsToggle.get_active();
        settings.symbols = symbolsToggle.get_active();
        settings.pronounceable = pronounceableToggle.get_active();

        return settings;
    }

    void NewPasswordPalette::PasswordGeneratorController::SaveCurrentSettings() {
        auto settings = GetSettings();

        Settings::PASSWORD_GENERATOR_DEFAULT_LENGTH.SetValue(settings.length);
        Settings::PASSWORD_GENERATOR_USE_LOWERCASE.SetValue(settings.lowercase);
        Settings::PASSWORD_GENERATOR_USE_UPPERCASE.SetValue(settings.uppercase);
        Settings::PASSWORD_GENERATOR_USE_NUMERALS.SetValue(settings.numerals);
        Settings::PASSWORD_GENERATOR_USE_SYMBOLS.SetValue(settings.symbols);
        Settings::PASSWORD_GENERATOR_PRONOUNCEABLE.SetValue(settings.pronounceable);
    }


#pragma endregion

#pragma region PALETTE
    NewPasswordPalette::NewPasswordPalette(const std::string& name) :
        DualChoicePalette(
            name.empty()
            ? _(NEW_PASSWORD_PALETTE_TITLE)
            : _(NEW_PASSWORD_PALETTE_EDIT_TITLE)
        ),

        passwordGeneratorController(PasswordGenerator::GetDefaultSettings()),

        mainVBox(Gtk::Orientation::VERTICAL),
        mainHBox(Gtk::Orientation::HORIZONTAL)
    {
        //set_default_size(530, 290);
        set_default_size(530, -1);

        bool isEditing = !(name.empty());

        SetYesSuggested();

        if (! isEditing) {
            titleLabel.set_text(_(NEW_PASSWORD_PALETTE_LABEL_TITLE));
        }
        else {
            titleLabel.set_text(_(NEW_PASSWORD_PALETTE_EDIT_LABEL_TITLE));
            newPasswordWidget.SetFixedName(name);
        }

        titleLabel.set_margin(10);
        mainVBox.append(titleLabel);

        newPasswordWidget.set_hexpand(true);
        newPasswordWidget.set_halign(Gtk::Align::CENTER);
        passwordGeneratorController.set_hexpand(true);
        passwordGeneratorController.set_halign(Gtk::Align::CENTER);
        newPasswordWidget.set_valign(Gtk::Align::START);
        passwordGeneratorController.set_valign(Gtk::Align::START);

        mainHBox.set_vexpand(true);

        mainHBox.append(newPasswordWidget);
        auto sep = Gtk::Separator(Gtk::Orientation::VERTICAL);
        sep.set_margin_start(10);
        sep.set_margin_end(10);
        mainHBox.append(sep);
        mainHBox.append(passwordGeneratorController);

        mainHBox.set_margin(10);
        mainHBox.set_margin_bottom(20);
        mainVBox.append(mainHBox);
        append(mainVBox);

        newPasswordWidget.SetSettingsProvider([this](){return passwordGeneratorController.GetSettings(); });

    }

    Password NewPasswordPalette::GetPassword() const {
        auto password = Password::FromPassword(newPasswordWidget.GetPassword());

        return password;
    }
    std::string NewPasswordPalette::GetPasswordName() const {
        return newPasswordWidget.GetPasswordName();
    }


#pragma endregion


}
