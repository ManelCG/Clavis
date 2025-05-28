#pragma once

#include <GUI/palettes/DualChoicePalette.h>

#include <gtkmm.h>

namespace Clavis::GUI {
    class SimpleEntryPalette : public DualChoicePalette<SimpleEntryPalette>{
    public:
        SimpleEntryPalette();

        std::string GetEntryText() const;
        void SetTitle(const std::string& title);
        void SetLabelText(const std::string& text);

    protected:

    private:
    private:
        Gtk::Box mainVBox;
        Gtk::Box mainHBox;

        Gtk::Label titleLabel;
        Gtk::Entry entry;
    };
}