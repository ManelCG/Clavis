#pragma once

#include <gtkmm.h>

#include <GUI/palettes/Palette.h>

#include <GUI/components/Tabber.h>

namespace Clavis::GUI {
    class DonatePalette : public Palette {
    public:
        DonatePalette();
    protected:

    private:
        Gtk::Box mainVBox;

        Gtk::Label titleLabel;

        Gtk::Label label1;
        Gtk::Label label2;
        Gtk::Label label3;
        Gtk::Label label4;
        Gtk::Label label5;

        Tabber donationsTabber;

        void AddDonationCard(const std::string& currency, Icons::IconDefinition icon, Icons::IconDefinition qr, const std::string& address, const std::string& openalias, int addressLabelLen);
    };
}