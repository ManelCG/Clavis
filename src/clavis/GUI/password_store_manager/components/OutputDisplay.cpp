#include <GUI/password_store_manager/components/OutputDisplay.h>

#include <error/ClavisError.h>
#include <language/Language.h>
#include <settings/Settings.h>
#include <system/Extensions.h>

namespace Clavis::GUI {
    OutputDisplay::OutputDisplay(const Glib::ustring& sectionTitle, const Glib::ustring& placeholder)
        : Gtk::Box(Gtk::Orientation::VERTICAL),
          defaultPlaceholder(placeholder),
          separator(Gtk::Orientation::HORIZONTAL),
          outputHbox(Gtk::Orientation::HORIZONTAL),
          hasContent(false) {
        set_margin_start(10);
        set_margin_end(10);
        set_margin_bottom(10);

        append(separator);

        sectionLabel.set_text(sectionTitle);
        sectionLabel.set_margin_top(5);
        sectionLabel.set_margin_bottom(5);
        append(sectionLabel);

        outputTextBox.set_editable(false);
        outputTextBox.set_hexpand(true);
        outputTextBox.set_placeholder_text(defaultPlaceholder);
        outputHbox.append(outputTextBox);

        copyButton.SetIcon(Icons::Actions::Copy);
        writeButton.SetIcon(Icons::Actions::Draw);
        copyButton.set_margin_start(5);
        writeButton.set_margin_start(5);
        outputHbox.append(copyButton);
        outputHbox.append(writeButton);

        append(outputHbox);

        copyButton.signal_clicked().connect([this]() {
            TryCopy();
        });

        writeButton.signal_clicked().connect([this]() {
            RaiseClavisError(_(ERROR_NOT_IMPLEMENTED));
        });

        // SetAction calls dispatcher.connect(), which is additive -- these must be set exactly
        // once, here in the constructor.
        clipboardClearTimeout.SetAction([this]() {
            Gdk::Display::get_default()->get_clipboard()->set_text("");
        });

        contentClearTimeout.SetAction([this]() {
            Clear();
        });

        set_visible(false);
    }

    void OutputDisplay::AppendTrailingWidget(Gtk::Widget& widget) {
        widget.set_margin_start(5);
        outputHbox.append(widget);
    }

    void OutputDisplay::SetOutputText(const std::string& text) {
        outputTextBox.set_text(text);
        hasContent = !text.empty();

        if (hasContent)
            set_visible(true);
    }

    void OutputDisplay::Clear() {
        OnClear();

        outputTextBox.set_text("");
        outputTextBox.set_placeholder_text(defaultPlaceholder);
        hasContent = false;

        set_visible(false);
    }

    bool OutputDisplay::HasContent() const {
        return hasContent;
    }

    void OutputDisplay::ArmClearTimeout() {
        const int clearSeconds = Settings::CLEAR_PASSWORD_DISPLAY_SECONDS.GetValue();
        if (clearSeconds <= 0)
            return;

        contentClearTimeout.SetSeconds(clearSeconds);
        contentClearTimeout.StartTimeout();
    }

    void OutputDisplay::TryCopy() {
        const auto text = GetClipboardText();
        if (text.empty()) {
            DisplayError();
            return;
        }

        if (!System::CopyToClipboard(text)) {
            DisplayError();
            return;
        }

        DisplaySuccess();

        const int clearSeconds = Settings::CLIPBOARD_CLEAR_SECONDS.GetValue();
        if (clearSeconds <= 0)
            return;

        clipboardClearTimeout.SetSeconds(clearSeconds);
        clipboardClearTimeout.StartTimeout();
    }

    void OutputDisplay::ShowDecryptionFailedPlaceholder() {
        outputTextBox.set_placeholder_text(_(PASSWORDSTORE_DECRYPTION_FAILED_HINT));
    }

    void OutputDisplay::DisplayError() {
        outputTextBox.DisplayError();
    }

    void OutputDisplay::DisplaySuccess() {
        outputTextBox.DisplaySuccess();
    }
}
