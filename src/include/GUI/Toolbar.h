#pragma once

#include <gtkmm/separator.h>
#include <gtkmm/box.h>

namespace Clavis::GUI {
    class Toolbar : public Gtk::Box {
    public:
        Toolbar();
        explicit Toolbar(Gtk::Align align);

        void AppendBegin(Gtk::Widget& w);
        void AppendMid(Gtk::Widget& w);
        void AppendEnd(Gtk::Widget& w);

        void SetBeginGrow(bool b);
        void SetMidGrow(bool b);
        void SetEndGrow(bool b);

        void SetHasSeparator(bool b);
        void SetSeparatorMargin(int margin);

    protected:

    private:
        Gtk::Box InnerBox;

        Gtk::Box LeftBox;
        Gtk::Box MidBox;
        Gtk::Box RightBox;

        Gtk::Separator Separator;

        Gtk::Align Align;
    };
}