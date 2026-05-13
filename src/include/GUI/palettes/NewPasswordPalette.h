#pragma once

#include <GUI/palettes/DualChoicePalette.h>
#include <password_store/PasswordGenerator.h>

#include <password_store/Password.h>

#include <GUI/components/typedEntries/IntEntry.h>
#include <GUI/components/IconButton.h>
#include <GUI/components/ToggleIconButton.h>

#include <GUI/components/RequiredEntry.h>

#include <gtkmm.h>

namespace Clavis::GUI {
    class NewPasswordPalette : public DualChoicePalette<NewPasswordPalette>{
    public:
        class NewPasswordWidget : public Gtk::Box {
        public:
            NewPasswordWidget();
            void SetSettingsProvider(std::function<PasswordGenerator::GeneratorSettings()> lambda);
            std::string GetPassword() const;
            std::string GetPasswordName() const;

            void SetFixedName(const std::string& name);
            void SetGeneratable(bool generatable);

            RequiredEntry nameEntry;
            RequiredEntry passwordEntry;

        private:
            std::function<PasswordGenerator::GeneratorSettings()> settingsProvider = [](){return PasswordGenerator::GeneratorSettings();};

            Gtk::Label setPasswordNameLabel;
            Gtk::Label newPasswordLabel;

            Gtk::Box passwordEntryHBox;

            IconButton generatePasswordButton;
            ToggleIconButton displayPasswordButton;
        };

        class PasswordGeneratorController : public Gtk::Box {
        public:
            PasswordGeneratorController(const PasswordGenerator::GeneratorSettings& settings);

            void PopulateWithValues(const PasswordGenerator::GeneratorSettings& settings);
            PasswordGenerator::GeneratorSettings GetSettings();
            void SetOnSettingsChanged(std::function<void()> callback);

        protected:

        private:
            void SaveCurrentSettings();

            std::function<void()> onSettingsChanged = [](){};

            Gtk::Label passwordGeneratorLabel;

            Gtk::Box lengthHBox;
            Gtk::Label lengthLabel;
            IntEntry lengthEntry;

            Gtk::Box bigHBox;
            Gtk::Box leftVBox;
            Gtk::Box rightVBox;

            Gtk::CheckButton lowercaseToggle;
            Gtk::CheckButton uppercaseToggle;
            Gtk::CheckButton numeralsToggle;
            Gtk::CheckButton symbolsToggle;
            Gtk::CheckButton pronounceableToggle;
        };

        NewPasswordPalette(const std::string& name);

        std::string GetPasswordName() const;
        Password GetPassword() const;

    protected:

    private:
        void DoGiveResponse(bool r) override;

        Gtk::Box mainVBox;
        Gtk::Box mainHBox;

        NewPasswordWidget newPasswordWidget;
        PasswordGeneratorController passwordGeneratorController;

        Gtk::Label titleLabel;
    };
}