#pragma once

#include <memory>
#include <string>
#include <vector>

#include <gtkmm.h>

#include <GUI/components/IconButton.h>

namespace Clavis::GUI {
    // Editable, numbered list of recovery codes: one row per stored code with a delete button,
    // plus a permanently visible input row with an add button.
    //
    // Return in the input row adds the code and leaves the cursor ready for the next one, and
    // pasting a multi-line block splits it into one code per line -- recovery codes are almost
    // always handed over as a block of lines to copy.
    class RecoveryCodeList : public Gtk::Box {
    public:
        RecoveryCodeList();
        ~RecoveryCodeList() override;

        void SetCodes(const std::vector<std::string>& codes);

        // Includes whatever is still sitting in the input row, so a code that was typed but not
        // explicitly added is never silently dropped.
        [[nodiscard]] std::vector<std::string> GetCodes() const;

    protected:

    private:
        struct Row {
            Gtk::Box box;
            Gtk::Label indexLabel;
            Gtk::Entry entry;
            IconButton deleteButton;
        };

        void AddCode(const std::string& code);
        void AddCodesFromText(const std::string& text);
        void CommitPendingInput();
        void RemoveRow(Row* row);

        // Numbering is 1-based and recomputed whenever the list changes, so it always reads as a
        // plain count of how many codes are stored.
        void Renumber();

        // Whether the viewport position needs restoring after a row change. Adding deliberately
        // does not touch the scroll position at all -- GTK's own behaviour there is fine, and
        // every attempt to improve on it fought the viewport's allocation and lost.
        enum class ScrollIntent {
            NONE,
            RESTORE,
        };

        void RequestScroll(ScrollIntent intent, double offset = 0.0);
        void ApplyPendingScroll();

        bool OnInputKeyPressed(guint keyval, guint keycode, Gdk::ModifierType state);

        std::vector<std::unique_ptr<Row>> rows;

        // The list owns its scroll area while the input row sits outside it, so adding codes
        // never scrolls the input out of reach.
        Gtk::ScrolledWindow rowsScroll;
        Gtk::Box rowsVBox;

        Gtk::Box inputHBox;
        Gtk::Label inputIndexLabel;
        Gtk::Entry inputEntry;
        IconButton addButton;

        ScrollIntent scrollIntent = ScrollIntent::NONE;
        double pendingScrollOffset = 0.0;

        // The restore is re-asserted on a short repeating tick rather than at one chosen moment.
        // Setting it from inside the adjustment's "changed" emission lands mid-allocation and the
        // viewport overwrites it afterwards, so the position has to be re-applied until the
        // layout has stopped moving.
        sigc::connection scrollTicker;
        gint64 scrollDeadlineUs = 0;

        // Guards the deferred row removal and scroll adjustments: both run from idle callbacks
        // rather than from inside a button handler, which would free the widget mid-signal.
        std::shared_ptr<bool> alive;
    };
}
