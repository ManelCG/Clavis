#pragma once

#include <map>
#include <vector>
#include <gtkmm/application.h>
#include <gtkmm/widget.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/entry.h>

namespace Clavis::GUI::Extensions {
    std::vector<Gtk::Widget*> GetAllChildren(Gtk::Widget* w, bool recursive = false);
    Gtk::Window* GetParentWindow(Gtk::Widget* widget);

    template<typename T>
    std::vector<T*> FilterWidgets(std::vector<Gtk::Widget*> widgets);

    Glib::RefPtr<Gtk::CssProvider> ApplyCss(Gtk::Widget* w, std::string css);
    std::vector<Glib::RefPtr<Gtk::CssProvider>> ApplyCss(std::vector<Gtk::Widget*>, std::string css);

    template<typename T>
    std::vector<T*> FilterWidgets(std::vector<Gtk::Widget*> widgets)
    {
        std::vector<T*> ret;

        for (auto w : widgets) {
            auto c = dynamic_cast<T*>(w);

            if (c != nullptr)
                ret.push_back(c);
        }

        return ret;
    }

    template<typename T>
    T* __SpawnWindow_IMPL(std::function<T*()> constructor, Gtk::Widget* parent, bool doSafeDelete) {
        auto palette = constructor();

        Gtk::Window* window = GetParentWindow(parent);
        if (window != nullptr) {
            palette->set_transient_for(*window);
            window->set_sensitive(false);
        }

        palette->set_modal(true);

        palette->signal_hide().connect([palette, window, doSafeDelete]() {
            if (window != nullptr)
                window->set_sensitive(true);

            if (doSafeDelete)
                delete palette;
        });

        palette->show();

        return palette;
    }

    template<class T>
    T* SpawnWindowNoSafeDelete(std::function<T*()> constructor, Gtk::Widget* parent = nullptr) {
        return __SpawnWindow_IMPL<T>(constructor, parent, false);
    }

    template<class T>
    T* SpawnWindow(std::function<T*()> constructor, Gtk::Widget* parent = nullptr) {
        return __SpawnWindow_IMPL<T>(constructor, parent, true);
    }

    template<class T>
    T* SpawnWindow(Gtk::Widget* parent = nullptr) {
        return SpawnWindow<T>([](){return new T();}, parent);
    }

}