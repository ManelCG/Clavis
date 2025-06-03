#pragma once

#include <filesystem>

#include <GUI/palettes/DualChoicePalette.h>

namespace Clavis::GUI {
    class ChoosePasswordStoreLocationPalette : public DualChoicePalette<ChoosePasswordStoreLocationPalette> {
    public:
        ChoosePasswordStoreLocationPalette();

        std::filesystem::path GetSelectedPath();

    protected:

    private:
        Gtk::Label choosePathLabel;
        Gtk::Entry choosePathEntry;

    };
}