#pragma once

#include <string>

#include <gtkmm.h>

#include <GUI/components/IconButton.h>
#include <GUI/components/RequiredEntry.h>
#include <GUI/signals/UniqueSignalTimeoutDispatcher.h>

namespace Clavis::GUI {
    // Shared chrome for the decrypted-output sections at the bottom of the window: separator,
    // section title, the read-only output entry, copy and write buttons, and the two safety
    // timeouts (clear the clipboard, clear the display).
    //
    // Sections start hidden and are shown only while something is decrypted.
    class OutputDisplay : public Gtk::Box {
    public:
        explicit OutputDisplay(const Glib::ustring& sectionTitle, const Glib::ustring& placeholder);

        // Wipes the secret, blanks the entry and hides the section.
        void Clear();

        void TryCopy();

        [[nodiscard]] bool HasContent() const;

    protected:
        // Subclasses wipe their own secret state here. Called by Clear().
        virtual void OnClear() = 0;

        // Text placed on the clipboard by TryCopy(). Empty means "nothing to copy".
        [[nodiscard]] virtual std::string GetClipboardText() const = 0;

        // Widget occupying the slot to the right of the write button -- the visibility toggle for
        // passwords, the countdown or HOTP controls for 2FA.
        void AppendTrailingWidget(Gtk::Widget& widget);

        void SetOutputText(const std::string& text);
        void ArmClearTimeout();

        // Swaps the hint for a failure message; Clear() puts the normal one back.
        void ShowDecryptionFailedPlaceholder();

        void DisplayError();
        void DisplaySuccess();

        RequiredEntry outputTextBox;
        IconButton copyButton;
        IconButton writeButton;

    private:
        // Members rather than locals: the section is shown and hidden at runtime, so its widgets
        // have to stay addressable.
        Glib::ustring defaultPlaceholder;

        Gtk::Separator separator;
        Gtk::Label sectionLabel;
        Gtk::Box outputHbox;

        UniqueSignalTimeoutDispatcher clipboardClearTimeout;
        UniqueSignalTimeoutDispatcher contentClearTimeout;

        bool hasContent;
    };
}
