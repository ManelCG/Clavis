#pragma once

#include <memory>

#include <gtkmm/dialog.h>

#include <GUI/Titlebar/Titlebar.h>

namespace Clavis::GUI {
    class Palette : public Gtk::Window {
    public:
        explicit Palette(Glib::ustring title);
    protected:

    private:
        std::shared_ptr<Titlebar> WindowTitlebar;

        void SetCustomTitlebar();
        void DisableShadows();

        bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    };
}
