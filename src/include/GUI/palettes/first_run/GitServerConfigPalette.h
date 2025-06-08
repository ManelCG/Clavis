#pragma once

#include <GUI/palettes/DualChoicePalette.h>

#include <gtkmm.h>

namespace Clavis::GUI {
    class GitServerConfigPalette : public DualChoicePalette<GitServerConfigPalette> {
    public:
        GitServerConfigPalette();

    protected:

    private:
        Gtk::Label notImplementedLabel;
    };
}