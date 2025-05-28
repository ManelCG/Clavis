#pragma once

#include <gtkmm.h>

namespace Clavis::GUI {
    template<typename T>
    class TypedEntry : public Gtk::Entry {
    public:
        TypedEntry() {
            // signal_insert_text().connect([this](const Glib::ustring &text, int *position) {
            //     OnEntryInsertText(text, position);
            // }, false);
        }
        virtual void OnEntryInsertText(const Glib::ustring& text, int* position) = 0;

        virtual T GetValue() = 0;
        virtual void SetValue(T value) = 0;
    };
}