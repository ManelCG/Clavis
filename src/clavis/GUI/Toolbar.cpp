#include <GUI/Toolbar.h>

namespace Clavis::GUI {
    Toolbar::Toolbar() : Toolbar(Gtk::Align::START) {}

    Toolbar::Toolbar(Gtk::Align align):
        LeftBox(Gtk::Orientation::HORIZONTAL),
        MidBox(Gtk::Orientation::HORIZONTAL),
        RightBox(Gtk::Orientation::HORIZONTAL)
    {
        set_orientation(Gtk::Orientation::VERTICAL);

        Align = align;

        append(InnerBox);
        InnerBox.set_orientation(Gtk::Orientation::HORIZONTAL);

        set_valign(Align);

        InnerBox.append(LeftBox);
        InnerBox.append(MidBox);
        InnerBox.append(RightBox);

        LeftBox.set_halign(Gtk::Align::START);
        MidBox.set_halign(Gtk::Align::CENTER);
        RightBox.set_halign(Gtk::Align::END);

        SetBeginGrow(true);
        SetMidGrow(true);
        SetEndGrow(true);

        MidBox.set_margin_start(5);
        MidBox.set_margin_end(5);

        if (Align == Gtk::Align::START)
            append(Separator);
        else
            prepend(Separator);
    }

    void Toolbar::AppendBegin(Gtk::Widget &w) {
        LeftBox.append(w);
    }

    void Toolbar::AppendMid(Gtk::Widget &w) {
        MidBox.append(w);
    }

    void Toolbar::AppendEnd(Gtk::Widget &w) {
        RightBox.append(w);
    }

    void Toolbar::SetBeginGrow(bool b) {
        LeftBox.set_hexpand(b);
    }
    void Toolbar::SetMidGrow(bool b) {
        MidBox.set_hexpand(b);
    }
    void Toolbar::SetEndGrow(bool b) {
        RightBox.set_hexpand(b);
    }

    void Toolbar::SetHasSeparator(bool b) {
        b ? Separator.show() : Separator.hide();
    }
    void Toolbar::SetSeparatorMargin(int margin) {
        if (Align == Gtk::Align::START)
            Separator.set_margin_top(margin);
        else
            Separator.set_margin_bottom(margin);

    }
}
