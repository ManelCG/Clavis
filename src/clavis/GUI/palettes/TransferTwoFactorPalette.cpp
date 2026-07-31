#include <GUI/palettes/TransferTwoFactorPalette.h>

#include <image/QRCode.h>
#include <language/Language.h>
#include <settings/Settings.h>
#include <system/Extensions.h>

namespace Clavis::GUI {
    TransferTwoFactorPalette::TransferTwoFactorPalette(const std::string& name, const TwoFactor::TwoFactorEntry& entry)
        : Palette(_(TRANSFER_TWO_FACTOR_PALETTE_TITLE, name)),
          entry(entry),
          mainVBox(Gtk::Orientation::VERTICAL),
          uriHBox(Gtk::Orientation::HORIZONTAL) {
        set_default_size(420, -1);
        mainVBox.set_margin(10);

        instructionsLabel.set_text(_(TRANSFER_TWO_FACTOR_PALETTE_INSTRUCTIONS));
        instructionsLabel.set_wrap(true);
        instructionsLabel.set_halign(Gtk::Align::CENTER);
        instructionsLabel.set_justify(Gtk::Justification::CENTER);
        mainVBox.append(instructionsLabel);

        warningLabel.set_text(_(TRANSFER_TWO_FACTOR_PALETTE_WARNING));
        warningLabel.set_wrap(true);
        warningLabel.set_halign(Gtk::Align::CENTER);
        warningLabel.set_justify(Gtk::Justification::CENTER);
        warningLabel.set_margin_top(5);
        warningLabel.add_css_class("dim-label");
        mainVBox.append(warningLabel);

        // Set(shared_ptr) rather than Set(Image&): the Pixbuf does not copy the pixel buffer, so
        // the picture has to keep the Image alive.
        qrImage = QRCode::Render(this->entry.BuildUri());
        qrPicture.Set(qrImage);
        qrPicture.set_can_shrink(false);
        qrPicture.set_halign(Gtk::Align::CENTER);
        qrPicture.set_margin_top(10);
        qrPicture.set_margin_bottom(10);
        mainVBox.append(qrPicture);

        uriLabel.set_text(_(TRANSFER_TWO_FACTOR_PALETTE_URI_LABEL));
        uriLabel.set_halign(Gtk::Align::START);
        mainVBox.append(uriLabel);

        uriEntry.set_text(this->entry.BuildUri());
        uriEntry.set_editable(false);
        uriEntry.set_hexpand(true);
        copyUriButton.SetIcon(Icons::Actions::Copy);
        copyUriButton.set_margin_start(5);
        uriHBox.append(uriEntry);
        uriHBox.append(copyUriButton);
        uriHBox.set_margin_top(5);
        mainVBox.append(uriHBox);

        clipboardClearTimeout.SetAction([]() {
            Gdk::Display::get_default()->get_clipboard()->set_text("");
        });

        copyUriButton.signal_clicked().connect([this]() {
            if (!System::CopyToClipboard(this->entry.BuildUri()))
                return;

            const int clearSeconds = Settings::CLIPBOARD_CLEAR_SECONDS.GetValue();
            if (clearSeconds <= 0)
                return;

            clipboardClearTimeout.SetSeconds(clearSeconds);
            clipboardClearTimeout.StartTimeout();
        });

        // The secret is on screen in scannable form, so the window closes itself on the same
        // timeout that governs the decrypted-output sections.
        autoCloseTimeout.SetAction([this]() {
            close();
        });

        if (const int clearSeconds = Settings::CLEAR_PASSWORD_DISPLAY_SECONDS.GetValue(); clearSeconds > 0) {
            autoCloseTimeout.SetSeconds(clearSeconds);
            autoCloseTimeout.StartTimeout();
        }

        set_child(mainVBox);
    }
}
