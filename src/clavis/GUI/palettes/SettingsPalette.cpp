#include <GUI/palettes/SettingsPalette.h>

#include <language/Language.h>
#include <settings/Settings.h>

namespace Clavis::GUI {

    // ── helpers ────────────────────────────────────────────────────────────

    Gtk::Grid& SettingsPalette::BeginSection(const Glib::ustring& title) {
        auto* frame = Gtk::make_managed<Gtk::Frame>(title);
        auto* grid  = Gtk::make_managed<Gtk::Grid>();
        grid->set_row_spacing(8);
        grid->set_column_spacing(16);
        grid->set_margin(10);
        frame->set_child(*grid);
        sectionsBox.append(*frame);
        return *grid;
    }

    void SettingsPalette::AddBoolRow(Gtk::Grid& grid, int row,
        const Glib::ustring& label,
        std::function<bool()> get, std::function<void(bool)> set)
    {
        auto* lbl   = Gtk::make_managed<Gtk::Label>(label);
        lbl->set_halign(Gtk::Align::START);
        lbl->set_hexpand(true);

        auto* check = Gtk::make_managed<Gtk::CheckButton>();
        check->set_active(get());
        check->set_halign(Gtk::Align::END);

        grid.attach(*lbl,   0, row);
        grid.attach(*check, 1, row);

        saveActions.push_back([check, set]() { set(check->get_active()); });
    }

    void SettingsPalette::AddIntRow(Gtk::Grid& grid, int row,
        const Glib::ustring& label,
        std::function<int()> get, std::function<void(int)> set,
        int min, int max)
    {
        auto* lbl  = Gtk::make_managed<Gtk::Label>(label);
        lbl->set_halign(Gtk::Align::START);
        lbl->set_hexpand(true);

        auto adj  = Gtk::Adjustment::create(get(), min, max, 1.0, 10.0);
        auto* spin = Gtk::make_managed<NoScrollSpinButton>(adj, 1.0, 0);
        spin->set_numeric(true);
        spin->set_halign(Gtk::Align::END);

        grid.attach(*lbl,  0, row);
        grid.attach(*spin, 1, row);

        saveActions.push_back([spin, set]() { set(spin->get_value_as_int()); });
    }

    void SettingsPalette::AddStringRow(Gtk::Grid& grid, int row,
        const Glib::ustring& label,
        std::function<std::string()> get,
        std::function<void(const std::string&)> set)
    {
        auto* lbl   = Gtk::make_managed<Gtk::Label>(label);
        lbl->set_halign(Gtk::Align::START);

        auto* entry = Gtk::make_managed<Gtk::Entry>();
        entry->set_text(get());
        entry->set_hexpand(true);

        grid.attach(*lbl,   0, row);
        grid.attach(*entry, 1, row);

        saveActions.push_back([entry, set]() { set(std::string(entry->get_text())); });
    }

    void SettingsPalette::AddCheckIntRow(Gtk::Grid& grid, int row,
        const Glib::ustring& label,
        std::function<int()> get, std::function<void(int)> set,
        int min, int max, int defaultWhenEnabled, const Glib::ustring& unit)
    {
        auto* lbl = Gtk::make_managed<Gtk::Label>(label);
        lbl->set_halign(Gtk::Align::START);
        lbl->set_hexpand(true);

        const int current = get();

        auto* hbox  = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
        auto* check = Gtk::make_managed<Gtk::CheckButton>();
        check->set_active(current > 0);
        auto adj    = Gtk::Adjustment::create(current > 0 ? current : defaultWhenEnabled,
                                              min, max, 1.0, 10.0);
        auto* spin  = Gtk::make_managed<NoScrollSpinButton>(adj, 1.0, 0);
        spin->set_numeric(true);
        spin->set_sensitive(current > 0);

        hbox->append(*check);
        if (!unit.empty()) {
            auto* unitLbl = Gtk::make_managed<Gtk::Label>(unit);
            unitLbl->set_sensitive(current > 0);
            hbox->append(*unitLbl);
            check->signal_toggled().connect([unitLbl, check]() {
                unitLbl->set_sensitive(check->get_active());
            });
        }
        hbox->append(*spin);
        hbox->set_halign(Gtk::Align::END);

        grid.attach(*lbl,  0, row);
        grid.attach(*hbox, 1, row);

        check->signal_toggled().connect([check, spin]() {
            spin->set_sensitive(check->get_active());
        });

        saveActions.push_back([check, spin, set]() {
            set(check->get_active() ? spin->get_value_as_int() : 0);
        });
    }

    void SettingsPalette::AddEnumRow(Gtk::Grid& grid, int row,
        const Glib::ustring& label,
        std::function<std::string()> get,
        std::function<void(const std::string&)> set,
        std::vector<std::pair<std::string, Glib::ustring>> options)
    {
        auto* lbl   = Gtk::make_managed<Gtk::Label>(label);
        lbl->set_halign(Gtk::Align::START);
        lbl->set_hexpand(true);

        auto* combo = Gtk::make_managed<NoScrollComboBoxText>();
        for (const auto& [id, text] : options)
            combo->append(id, text);
        combo->set_active_id(get());
        combo->set_halign(Gtk::Align::END);

        grid.attach(*lbl,   0, row);
        grid.attach(*combo, 1, row);

        saveActions.push_back([combo, set]() {
            auto id = std::string(combo->get_active_id());
            if (!id.empty())
                set(id);
        });
    }

    // ── constructor ────────────────────────────────────────────────────────

    SettingsPalette::SettingsPalette()
        : DualChoicePalette<SettingsPalette>(_(SETTINGS_PALETTE_TITLE)),
          sectionsBox(Gtk::Orientation::VERTICAL)
    {
        set_default_size(520, 580);
        set_resizable(true);

        SetYesText(_(MISC_SAVE_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));
        SetYesSuggested();

        sectionsBox.set_spacing(8);
        sectionsBox.set_margin(10);

        scrolledWindow.set_child(sectionsBox);
        scrolledWindow.set_vexpand(true);
        scrolledWindow.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
        append(scrolledWindow);

        int row;

        // ── General ────────────────────────────────────────────────────────
        {
            auto& grid = BeginSection(_(SETTINGS_SECTION_GENERAL));
            row = 0;

            std::vector<std::pair<std::string, Glib::ustring>> langOptions;
            for (auto lang : Language::GetAllLanguages())
                langOptions.emplace_back(Language::GetLanguageCode(lang), Language::GetLanguageName(lang));

            AddEnumRow(grid, row++, _(SETTINGS_LANGUAGE_LABEL),
                []()                       { return Settings::CLAVIS_LANGUAGE.GetValue(); },
                [](const std::string& v)   { Settings::CLAVIS_LANGUAGE.SetValue(v); },
                langOptions);
        }

        // ── Appearance ─────────────────────────────────────────────────────
        {
            auto& grid = BeginSection(_(SETTINGS_SECTION_APPEARANCE));
            row = 0;

            AddBoolRow(grid, row++, _(SETTINGS_DO_USE_DARK_THEME_LABEL),
                []()       { return Settings::DO_USE_DARK_THEME.GetValue(); },
                [](bool v) { Settings::DO_USE_DARK_THEME.SetValue(v); });

            AddEnumRow(grid, row++, _(SETTINGS_WINDOW_DECORATIONS_LABEL),
                []()                       { return Settings::WINDOW_DECORATIONS.GetValue(); },
                [](const std::string& v)   { Settings::WINDOW_DECORATIONS.SetValue(v); },
                {
                    { std::string(Settings::WINDOW_DECORATIONS.CLAVIS_CSD),   _(SETTINGS_WINDOW_DECORATIONS_CLAVIS_CSD)   },
                    { std::string(Settings::WINDOW_DECORATIONS.GTK_CSD),      _(SETTINGS_WINDOW_DECORATIONS_GTK_CSD)      },
                    { std::string(Settings::WINDOW_DECORATIONS.FORCE_NO_CSD), _(SETTINGS_WINDOW_DECORATIONS_FORCE_NO_CSD) },
                });

            AddBoolRow(grid, row++, _(SETTINGS_FORCE_CLAVIS_STYLE_LABEL),
                []()       { return Settings::FORCE_CLAVIS_STYLE.GetValue(); },
                [](bool v) { Settings::FORCE_CLAVIS_STYLE.SetValue(v); });

            AddBoolRow(grid, row++, _(SETTINGS_DISABLE_SHADOWS_LABEL),
                []()       { return Settings::DISABLE_SHADOWS.GetValue(); },
                [](bool v) { Settings::DISABLE_SHADOWS.SetValue(v); });

#ifdef __WINDOWS__
            AddStringRow(grid, row++, _(SETTINGS_CLAVIS_THEME_LABEL),
                []()                       { return Settings::CLAVIS_THEME.GetValue(); },
                [](const std::string& v)   { Settings::CLAVIS_THEME.SetValue(v); });

            AddStringRow(grid, row++, _(SETTINGS_CLAVIS_FONT_LABEL),
                []()                       { return Settings::CLAVIS_FONT.GetValue(); },
                [](const std::string& v)   { Settings::CLAVIS_FONT.SetValue(v); });
#endif
        }

        // ── Security ───────────────────────────────────────────────────────
        {
            auto& grid = BeginSection(_(SETTINGS_SECTION_SECURITY));
            row = 0;

            AddCheckIntRow(grid, row++, _(SETTINGS_CLEAR_PASSWORD_DISPLAY_LABEL),
                []()      { return Settings::CLEAR_PASSWORD_DISPLAY_SECONDS.GetValue(); },
                [](int v) { Settings::CLEAR_PASSWORD_DISPLAY_SECONDS.SetValue(v); },
                1, 300, 45, _(MISC_SECONDS));

            AddCheckIntRow(grid, row++, _(SETTINGS_CLIPBOARD_CLEAR_ENABLED_LABEL),
                []()      { return Settings::CLIPBOARD_CLEAR_SECONDS.GetValue(); },
                [](int v) { Settings::CLIPBOARD_CLEAR_SECONDS.SetValue(v); },
                1, 300, 45, _(MISC_SECONDS));
        }

        // ── Password Filter ────────────────────────────────────────────────
        {
            auto& grid = BeginSection(_(SETTINGS_SECTION_PASSWORD_BROWSER));
            row = 0;

            AddBoolRow(grid, row++, _(SETTINGS_FILTER_CASE_SENSITIVE_LABEL),
                []()       { return Settings::FILTER_CASE_SENSITIVE.GetValue(); },
                [](bool v) { Settings::FILTER_CASE_SENSITIVE.SetValue(v); });

            AddBoolRow(grid, row++, _(SETTINGS_SHOW_HIDDEN_FILES_LABEL),
                []()       { return Settings::SHOW_HIDDEN_FILES.GetValue(); },
                [](bool v) { Settings::SHOW_HIDDEN_FILES.SetValue(v); });
        }

        // ── Password Generator ─────────────────────────────────────────────
        {
            auto& grid = BeginSection(_(SETTINGS_SECTION_PASSWORD_GENERATOR));
            row = 0;

            AddIntRow(grid, row++, _(SETTINGS_PASSWORD_GENERATOR_LENGTH_LABEL),
                []()       { return Settings::PASSWORD_GENERATOR_DEFAULT_LENGTH.GetValue(); },
                [](int v)  { Settings::PASSWORD_GENERATOR_DEFAULT_LENGTH.SetValue(v); },
                1, 256);

            AddBoolRow(grid, row++, _(SETTINGS_PASSWORD_GENERATOR_LOWERCASE_LABEL),
                []()       { return Settings::PASSWORD_GENERATOR_USE_LOWERCASE.GetValue(); },
                [](bool v) { Settings::PASSWORD_GENERATOR_USE_LOWERCASE.SetValue(v); });

            AddBoolRow(grid, row++, _(SETTINGS_PASSWORD_GENERATOR_UPPERCASE_LABEL),
                []()       { return Settings::PASSWORD_GENERATOR_USE_UPPERCASE.GetValue(); },
                [](bool v) { Settings::PASSWORD_GENERATOR_USE_UPPERCASE.SetValue(v); });

            AddBoolRow(grid, row++, _(SETTINGS_PASSWORD_GENERATOR_NUMERALS_LABEL),
                []()       { return Settings::PASSWORD_GENERATOR_USE_NUMERALS.GetValue(); },
                [](bool v) { Settings::PASSWORD_GENERATOR_USE_NUMERALS.SetValue(v); });

            AddBoolRow(grid, row++, _(SETTINGS_PASSWORD_GENERATOR_SYMBOLS_LABEL),
                []()       { return Settings::PASSWORD_GENERATOR_USE_SYMBOLS.GetValue(); },
                [](bool v) { Settings::PASSWORD_GENERATOR_USE_SYMBOLS.SetValue(v); });

            AddBoolRow(grid, row++, _(SETTINGS_PASSWORD_GENERATOR_PRONOUNCEABLE_LABEL),
                []()       { return Settings::PASSWORD_GENERATOR_PRONOUNCEABLE.GetValue(); },
                [](bool v) { Settings::PASSWORD_GENERATOR_PRONOUNCEABLE.SetValue(v); });
        }

#ifdef __DEBUG__
        // ── Developer ──────────────────────────────────────────────────────
        {
            auto& grid = BeginSection(_(SETTINGS_SECTION_DEVELOPER));
            row = 0;

            AddBoolRow(grid, row++, _(SETTINGS_RUN_GTK_CSS_INSPECTOR_LABEL),
                []()       { return Settings::RUN_GTK_CSS_INSPECTOR.GetValue(); },
                [](bool v) { Settings::RUN_GTK_CSS_INSPECTOR.SetValue(v); });
        }
#endif
    }

    // ── save ───────────────────────────────────────────────────────────────

    void SettingsPalette::DoGiveResponse(bool r) {
        if (r) {
            for (auto& action : saveActions)
                action();
        }
        DualChoicePalette<SettingsPalette>::DoGiveResponse(r);
    }

}
