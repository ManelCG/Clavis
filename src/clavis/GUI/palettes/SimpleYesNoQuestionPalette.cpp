#include <GUI/palettes/SimpleYesNoQuestionPalette.h>

#include <language/Language.h>

namespace Clavis::GUI {
    SimpleYesNoQuestionPalette::SimpleYesNoQuestionPalette() :
        DualChoicePalette("")
    {
        set_default_size(300, 100);
    }

    void SimpleYesNoQuestionPalette::AddText(const std::string &text) {
        labels.push_back(Gtk::Label(text));

        labels.back().set_margin(10);
        append(labels.back());
    }

    void SimpleYesNoQuestionPalette::SetTitle(const std::string &title) {
        set_title(title);
    }


}