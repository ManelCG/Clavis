#pragma once

#include <gtkmm.h>

#include <GUI/components/LabeledIconButton.h>

namespace Clavis::GUI {
    class Tabber : public Gtk::Box {
    public:
        Tabber();

        void AddTab(const Glib::ustring& label, Widget* page);
        void AddTab(const Glib::ustring& label, const Icons::IconDefinition& icon, Widget* page);

    protected:

    private:
        std::vector<LabeledIconButton*> tabs;
        std::vector<Widget*> pages;

        void Show(int index) const;
        void HideAll() const;

        Gtk::Box tabsHBox;

    };
}