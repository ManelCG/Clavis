#include <GUI/components/typedEntries/IntEntry.h>

namespace Clavis::GUI {
    bool contains_only_numbers(const Glib::ustring& text) {
        for(int i = 0; i < text.length(); i++) {
            if(Glib::Unicode::isdigit(text[i]) == false)
                return false;
        }

        return true;
    }
    void IntEntry::OnEntryInsertText(const Glib::ustring &text, int *position) {
        if (contains_only_numbers(text))
            Gtk::Entry::on_insert_text(text, position);
    }

    int IntEntry::GetValue() {
        auto text = get_text();
        if (text.empty())
            return 0;

        try {
            return std::stoi(text);
        } catch (...) {
            return 0;
        }
    }

    void IntEntry::SetValue(int value) {
        set_text(std::to_string(value));
    }




}