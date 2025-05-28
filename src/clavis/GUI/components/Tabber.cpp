#include <GUI/components/Tabber.h>

namespace Clavis::GUI {
    Tabber::Tabber() {
        set_orientation(Gtk::Orientation::VERTICAL);
        tabsHBox.set_orientation(Gtk::Orientation::HORIZONTAL);

        append(tabsHBox);
    }

    void Tabber::AddTab(const Glib::ustring &label, const Icons::IconDefinition &icon, Widget* page) {
        AddTab(label, page);
        tabs.back()->SetIcon(icon);
    }

    void Tabber::AddTab(const Glib::ustring &label, Widget* page) {
        const auto tab = Gtk::make_managed<LabeledIconButton>();
        tab->SetLabel(label);
        tab->property_width_request().set_value(90);

        int tabID = tabs.size();

        tabs.push_back(tab);
        pages.push_back(page);

        tabsHBox.append(*tab);
        append(*page);
        page->hide();

        if (tabID == 0)
            Show(0);

        tab->signal_clicked().connect([this, tabID]() {
            HideAll();
            Show(tabID);
        });
    }

    void Tabber::HideAll() const {
        for (const auto& page : pages)
            page->hide();
        for (const auto& tab : tabs)
            tab->remove_css_class("suggested-action");
    }
    void Tabber::Show(int index) const {
        pages[index]->show();
        tabs[index]->add_css_class("suggested-action");
    }




}