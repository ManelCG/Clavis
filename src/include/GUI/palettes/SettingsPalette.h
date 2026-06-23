#pragma once

#include <functional>
#include <vector>

#include <gtkmm.h>

#include <GUI/palettes/DualChoicePalette.h>
#include <GUI/components/NoScrollWidgets.h>

namespace Clavis::GUI {
    class SettingsPalette : public DualChoicePalette<SettingsPalette> {
    public:
        SettingsPalette();

    protected:
        void DoGiveResponse(bool r) override;

    private:
        Gtk::ScrolledWindow scrolledWindow;
        Gtk::Box sectionsBox;

        Gtk::Grid& BeginSection(const Glib::ustring& title);

        void AddBoolRow(Gtk::Grid& grid, int row, const Glib::ustring& label,
                        std::function<bool()> get, std::function<void(bool)> set);
        void AddIntRow(Gtk::Grid& grid, int row, const Glib::ustring& label,
                       std::function<int()> get, std::function<void(int)> set,
                       int min, int max);
        // Checkbox + spinbutton on one row. get/set use 0 to mean "disabled".
        // unit is shown after the spinbutton (e.g. "seconds"); pass "" to omit.
        void AddCheckIntRow(Gtk::Grid& grid, int row, const Glib::ustring& label,
                            std::function<int()> get, std::function<void(int)> set,
                            int min, int max, int defaultWhenEnabled,
                            const Glib::ustring& unit = "");
        void AddStringRow(Gtk::Grid& grid, int row, const Glib::ustring& label,
                          std::function<std::string()> get,
                          std::function<void(const std::string&)> set);
        void AddEnumRow(Gtk::Grid& grid, int row, const Glib::ustring& label,
                        std::function<std::string()> get,
                        std::function<void(const std::string&)> set,
                        std::vector<std::pair<std::string, Glib::ustring>> options);

        std::vector<std::function<void()>> saveActions;
    };
}
