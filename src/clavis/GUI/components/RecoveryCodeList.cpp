#include <GUI/components/RecoveryCodeList.h>

#include <algorithm>

#include <extensions/StringHelper.h>
#include <language/Language.h>

namespace Clavis::GUI {
    namespace {
        // Wide enough for a three-digit count without the entries shifting as the list grows.
        constexpr int INDEX_LABEL_WIDTH_CHARS = 4;

        // How long a requested scroll keeps being re-applied as the layout settles, and how
        // often it is re-asserted within that window.
        constexpr gint64 SCROLL_INTENT_LIFETIME_US = 400 * 1000;
        constexpr unsigned int SCROLL_TICK_INTERVAL_MS = 25;

        Glib::ustring FormatIndex(size_t oneBasedIndex) {
            return Glib::ustring(std::to_string(oneBasedIndex)) + ".";
        }
    }

    RecoveryCodeList::RecoveryCodeList()
        : Gtk::Box(Gtk::Orientation::VERTICAL),
          rowsVBox(Gtk::Orientation::VERTICAL),
          inputHBox(Gtk::Orientation::HORIZONTAL),
          alive(std::make_shared<bool>(true)) {
        rowsScroll.set_child(rowsVBox);
        rowsScroll.set_has_frame(true);
        rowsScroll.set_vexpand(true);
        rowsScroll.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
        rowsVBox.set_valign(Gtk::Align::START);

        inputIndexLabel.set_width_chars(INDEX_LABEL_WIDTH_CHARS);
        inputIndexLabel.set_xalign(1.0f);
        inputIndexLabel.set_margin_end(5);
        inputIndexLabel.add_css_class("dim-label");

        inputEntry.set_placeholder_text(_(NEW_TWO_FACTOR_PALETTE_RECOVERY_ADD_HINT));
        inputEntry.set_hexpand(true);

        addButton.SetIcon(Icons::Plus);
        addButton.set_tooltip_text(_(NEW_TWO_FACTOR_PALETTE_RECOVERY_ADD_TOOLTIP));
        addButton.set_margin_start(5);

        inputHBox.append(inputIndexLabel);
        inputHBox.append(inputEntry);
        inputHBox.append(addButton);
        inputHBox.set_margin_top(5);

        append(rowsScroll);
        append(inputHBox);

        // The adjustment emits "changed" when its extents are recomputed after a row is added or
        // removed. That is the only point at which a scroll position can be set meaningfully, so
        // pending scrolls are applied from here rather than from an idle callback racing layout.
        if (const auto adjustment = rowsScroll.get_vadjustment())
            adjustment->signal_changed().connect([this]() {
                ApplyPendingScroll();
            });

        Renumber();

        addButton.signal_clicked().connect([this]() {
            CommitPendingInput();
        });

        // Return commits the code and leaves focus in the (now empty) input, so a block of codes
        // can be typed without touching the mouse.
        inputEntry.signal_activate().connect([this]() {
            CommitPendingInput();
        });

        const auto keyController = Gtk::EventControllerKey::create();
        keyController->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
        keyController->signal_key_pressed().connect(
            sigc::mem_fun(*this, &RecoveryCodeList::OnInputKeyPressed), false);
        inputEntry.add_controller(keyController);
    }

    RecoveryCodeList::~RecoveryCodeList() {
        *alive = false;
        scrollTicker.disconnect();
    }

    bool RecoveryCodeList::OnInputKeyPressed(guint keyval, guint keycode, Gdk::ModifierType state) {
        if (state != Gdk::ModifierType::CONTROL_MASK || (keyval != GDK_KEY_v && keyval != GDK_KEY_V))
            return false;

        // A single-line Gtk::Entry silently collapses pasted newlines, so the paste is handled
        // here instead: multi-line content becomes one code per line.
        const auto clipboard = Gdk::Display::get_default()->get_clipboard();
        auto aliveFlag = alive;

        clipboard->read_text_async([this, clipboard, aliveFlag](Glib::RefPtr<Gio::AsyncResult>& result) {
            if (!*aliveFlag)
                return;

            std::string text;
            try {
                text = clipboard->read_text_finish(result);
            } catch (const Glib::Error&) {
                return;
            }

            if (text.find('\n') == std::string::npos && text.find('\r') == std::string::npos) {
                // Ordinary single-line paste: insert at the cursor like GTK would have.
                // insert_text works in characters and returns how many it inserted, which is not
                // the same as the byte length once the text is not pure ASCII.
                const int position = inputEntry.get_position();
                const auto inserted = inputEntry.get_buffer()->insert_text(position, text);
                inputEntry.set_position(position + static_cast<int>(inserted));
                return;
            }

            AddCodesFromText(text);
        });

        return true;
    }

    void RecoveryCodeList::AddCodesFromText(const std::string& text) {
        auto normalized = text;
        std::replace(normalized.begin(), normalized.end(), '\r', '\n');

        for (const auto& line : StringHelper::Split(normalized, '\n', false))
            AddCode(line);

        Renumber();
    }

    void RecoveryCodeList::CommitPendingInput() {
        const auto text = inputEntry.get_text();
        if (StringHelper::Trim(text).empty())
            return;

        AddCodesFromText(text);

        inputEntry.set_text("");
        inputEntry.grab_focus();
    }

    void RecoveryCodeList::AddCode(const std::string& code) {
        const auto trimmed = StringHelper::Trim(code);
        if (trimmed.empty())
            return;

        auto row = std::make_unique<Row>();
        row->box.set_orientation(Gtk::Orientation::HORIZONTAL);

        row->indexLabel.set_width_chars(INDEX_LABEL_WIDTH_CHARS);
        row->indexLabel.set_xalign(1.0f);
        row->indexLabel.set_margin_end(5);
        row->indexLabel.add_css_class("dim-label");

        row->entry.set_text(trimmed);
        row->entry.set_hexpand(true);

        row->deleteButton.SetIcon(Icons::Actions::Trash);
        row->deleteButton.set_tooltip_text(_(NEW_TWO_FACTOR_PALETTE_RECOVERY_DELETE_TOOLTIP));
        row->deleteButton.set_margin_start(5);

        row->box.append(row->indexLabel);
        row->box.append(row->entry);
        row->box.append(row->deleteButton);
        row->box.set_margin_top(2);
        row->box.set_margin_bottom(2);
        row->box.set_margin_start(2);
        row->box.set_margin_end(2);

        auto* rowPtr = row.get();
        auto aliveFlag = alive;

        row->deleteButton.signal_clicked().connect([this, rowPtr, aliveFlag]() {
            // Deferred: destroying the row here would free the button while its own handler is
            // still on the stack.
            Glib::signal_idle().connect_once([this, rowPtr, aliveFlag]() {
                if (!*aliveFlag)
                    return;

                RemoveRow(rowPtr);
            });
        });

        rowsVBox.append(row->box);

        rows.push_back(std::move(row));
    }

    void RecoveryCodeList::RemoveRow(Row* row) {
        const auto it = std::find_if(rows.begin(), rows.end(),
                                     [row](const auto& candidate) { return candidate.get() == row; });

        if (it == rows.end())
            return;

        const auto adjustment = rowsScroll.get_vadjustment();
        const double previousOffset = adjustment ? adjustment->get_value() : 0.0;

        // A ScrolledWindow scrolls to keep the focused widget visible. Destroying the focused
        // delete button hands focus to the first focusable child -- the top row -- and GTK then
        // jumps the viewport there, overriding anything set afterwards. Moving focus out of the
        // row before it dies is what actually stops the jump.
        inputEntry.grab_focus();

        rowsVBox.remove((*it)->box);
        rows.erase(it);

        Renumber();
        RequestScroll(ScrollIntent::RESTORE, previousOffset);
    }

    void RecoveryCodeList::RequestScroll(ScrollIntent intent, double offset) {
        scrollIntent = intent;
        pendingScrollOffset = offset;

        ApplyPendingScroll();

        // Re-assert the restored position on a tick until the layout settles. Applying it once --
        // whether from an idle, or from the adjustment's "changed" signal -- is not enough:
        // "changed" is emitted partway through the viewport's allocation, so a value set there is
        // computed from extents that are still stale and is then overwritten when the allocation
        // finishes. Ticking sidesteps the ordering question entirely.
        scrollDeadlineUs = g_get_monotonic_time() + SCROLL_INTENT_LIFETIME_US;

        if (scrollTicker.connected())
            return;

        auto aliveFlag = alive;
        scrollTicker = Glib::signal_timeout().connect(
            [this, aliveFlag]() -> bool {
                if (!*aliveFlag)
                    return false;

                ApplyPendingScroll();

                if (g_get_monotonic_time() < scrollDeadlineUs)
                    return true;

                scrollIntent = ScrollIntent::NONE;
                return false;
            },
            SCROLL_TICK_INTERVAL_MS);
    }

    void RecoveryCodeList::ApplyPendingScroll() {
        if (scrollIntent == ScrollIntent::NONE)
            return;

        const auto adjustment = rowsScroll.get_vadjustment();
        if (!adjustment)
            return;

        const double maximum = std::max(0.0, adjustment->get_upper() - adjustment->get_page_size());

        adjustment->set_value(std::min(pendingScrollOffset, maximum));
    }

    void RecoveryCodeList::Renumber() {
        for (size_t i = 0; i < rows.size(); i++)
            rows[i]->indexLabel.set_text(FormatIndex(i + 1));

        // The input row carries the number the next code will get.
        inputIndexLabel.set_text(FormatIndex(rows.size() + 1));
    }

    void RecoveryCodeList::SetCodes(const std::vector<std::string>& codes) {
        for (const auto& row : rows)
            rowsVBox.remove(row->box);

        rows.clear();
        inputEntry.set_text("");

        for (const auto& code : codes)
            AddCode(code);

        Renumber();
    }

    std::vector<std::string> RecoveryCodeList::GetCodes() const {
        std::vector<std::string> codes;

        for (const auto& row : rows) {
            if (const auto trimmed = StringHelper::Trim(row->entry.get_text()); !trimmed.empty())
                codes.push_back(trimmed);
        }

        // A code typed into the input but never explicitly added still counts -- losing it just
        // because the user pressed OK instead of "+" would be a nasty surprise.
        if (const auto pending = StringHelper::Trim(inputEntry.get_text()); !pending.empty())
            codes.push_back(pending);

        return codes;
    }
}
