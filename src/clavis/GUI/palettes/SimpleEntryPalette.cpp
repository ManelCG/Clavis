#include <GUI/palettes/SimpleEntryPalette.h>

#include <language/Language.h>

namespace Clavis::GUI {
    SimpleEntryPalette::SimpleEntryPalette() :
        DualChoicePalette("placeholder"),

        mainVBox(Gtk::Orientation::VERTICAL),
        mainHBox(Gtk::Orientation::HORIZONTAL)
    {
        titleLabel.set_text("placeholder");
        titleLabel.set_margin(10);

        entry.set_margin(10);
        entry.set_margin_top(0);

        mainVBox.append(titleLabel);
        mainVBox.append(entry);

        mainHBox.set_margin(10);
        mainVBox.append(mainHBox);
        append(mainVBox);
    }

    std::string SimpleEntryPalette::GetEntryText() const {
        return entry.get_text();
    }

    void SimpleEntryPalette::SetTitle(const std::string &title) {
        set_title(title);
    }
    void SimpleEntryPalette::SetLabelText(const std::string &text) {
        titleLabel.set_text(text);
    }



}