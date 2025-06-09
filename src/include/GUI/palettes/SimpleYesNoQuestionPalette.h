#pragma once

#include <GUI/palettes/DualChoicePalette.h>

namespace Clavis::GUI {
    class SimpleYesNoQuestionPalette : public DualChoicePalette<SimpleYesNoQuestionPalette> {
    public:
        SimpleYesNoQuestionPalette();

        void SetTitle(const std::string& title);
        void AddText(const std::string& text);

    public:

    protected:

    private:
        std::vector<Gtk::Label> labels;
    };
}