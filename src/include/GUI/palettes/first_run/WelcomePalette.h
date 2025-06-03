#pragma once
#include <gtkmm/comboboxtext.h>
#include <GUI/palettes/DualChoicePalette.h>

namespace Clavis::GUI {
    class WelcomePalette : public DualChoicePalette<WelcomePalette> {
    public:
        WelcomePalette();

        LanguagesEnum GetSelectedLanguage();

    protected:

    private:
        Gtk::Label labelText1;
        Gtk::Label labelText2;

        Gtk::Box languageHBox;
        Gtk::Label languageLabel;
        Gtk::ComboBoxText languageComboBox;

        void UpdatePaletteLanguage();
        void SetPaletteTexts();
    };
}
