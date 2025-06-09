#pragma once

#include <gtkmm.h>

#include <GUI/palettes/Palette.h>
#include <GUI/components/PictureInsert.h>

namespace Clavis::GUI {
    class AboutPalette : public Palette {
    public:
        AboutPalette();

    protected:

    private:
        Gtk::Box mainVBox;

        PictureInsert logo;

        Gtk::Label clavisLabel;
        Gtk::Label versionLabel;
        Gtk::Label descriptionLabel;
        Gtk::Label githubLabel;
        Gtk::Label copyrightLabel;

        Gtk::Box buttonsHBox;
        Gtk::Button creditsButton;
        Gtk::Button licenseButton;
        Gtk::Button donateButton;
        Gtk::Button closeButton;
    };
}