#include <extensions/GUIExtensions.h>

namespace Clavis::GUI::Extensions {
    std::vector<Gtk::Widget*> GetAllChildren(Gtk::Widget* w, bool recursive)
    {
        std::vector<Gtk::Widget*> ret;

        for (auto c = w->get_first_child(); c != nullptr; c = c->get_next_sibling()) {
            ret.push_back(c);

            if (recursive) {
                auto rec = GetAllChildren(c, recursive);
                for (auto r : rec)
                    ret.push_back(r);
            }
        }

        return ret;
    }

    Glib::RefPtr<Gtk::CssProvider> ApplyCss(Gtk::Widget* w, std::string css) {
        auto provider = Gtk::CssProvider::create();
        provider->load_from_data(css);
        w->get_style_context()->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        return provider;
    }

    std::vector<Glib::RefPtr<Gtk::CssProvider>> ApplyCss(std::vector<Gtk::Widget*> ws, std::string css){
        std::vector<Glib::RefPtr<Gtk::CssProvider>> ret;

        for (auto w : ws)
            ret.push_back(ApplyCss(w, css));

        return ret;
    }
}