#include <gtkmm/separator.h>
#include <GUI/palettes/first_run/WelcomePalette.h>

#include <settings/Settings.h>

namespace Clavis::GUI {
    WelcomePalette::WelcomePalette() :
        DualChoicePalette(_(FIRST_RUN_WORKFLOW_TITLE))
    {
        set_margin(10);
        set_default_size(420, -1);

        SetPaletteTexts();


        labelText1.set_margin(5);

        for (const auto& lang : GetAllLanguages())
            languageComboBox.append(GetLanguageCode(lang), GetLanguageName(lang));
        languageComboBox.set_active_id(Settings::CLAVIS_LANGUAGE.GetValue());
        languageComboBox.set_margin_start(5);

        languageHBox.append(languageLabel);
        languageHBox.append(languageComboBox);

        append(labelText1);
        append(labelText2);

        auto sep = Gtk::Separator(Gtk::Orientation::HORIZONTAL);
        sep.set_margin(5);
        append(sep);

        append(languageHBox);

        languageHBox.set_hexpand(true);
        languageHBox.set_halign(Gtk::Align::CENTER);

        languageComboBox.signal_changed().connect([this]() {
            UpdatePaletteLanguage();
        });
    }

    LanguagesEnum WelcomePalette::GetSelectedLanguage() {
        auto code = languageComboBox.get_active_id();
        return GetLanguageFromCode(code);
    }

    void WelcomePalette::UpdatePaletteLanguage() {
        auto lang = GetSelectedLanguage();
        auto code = GetLanguageCode(lang);
        Settings::CLAVIS_LANGUAGE.SetValue(code);

        SetPaletteTexts();
    }

    void WelcomePalette::SetPaletteTexts() {
        set_title(_(FIRST_RUN_WORKFLOW_TITLE));
        SetYesText(_(MISC_ACCEPT_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));

        labelText1.set_text(_(FIRST_RUN_WORKFLOW_LABEL_1));
        labelText2.set_text(_(FIRST_RUN_WORKFLOW_LABEL_2));

        languageLabel.set_text(_(MISC_LANGUAGE));
    }




}