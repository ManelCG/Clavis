#pragma once

#include <memory>
#include <string>

#include <gtkmm.h>

#include <image/image.h>
#include <two_factor/TwoFactorEntry.h>

#include <GUI/components/IconButton.h>
#include <GUI/components/PictureInsert.h>
#include <GUI/palettes/Palette.h>
#include <GUI/signals/UniqueSignalTimeoutDispatcher.h>

namespace Clavis::GUI {
    // Hands the credential to another authenticator, as a scannable QR code plus the raw
    // otpauth:// URI. Both reveal the shared secret, hence the warning and the auto-close.
    class TransferTwoFactorPalette : public Palette {
    public:
        TransferTwoFactorPalette(const std::string& name, const TwoFactor::TwoFactorEntry& entry);

    protected:

    private:
        TwoFactor::TwoFactorEntry entry;

        // Must outlive qrPicture: the underlying Gdk::Pixbuf borrows this buffer.
        std::shared_ptr<Image> qrImage;

        Gtk::Box mainVBox;
        Gtk::Label instructionsLabel;
        Gtk::Label warningLabel;
        PictureInsert qrPicture;
        Gtk::Label uriLabel;
        Gtk::Box uriHBox;
        Gtk::Entry uriEntry;
        IconButton copyUriButton;

        UniqueSignalTimeoutDispatcher clipboardClearTimeout;
        UniqueSignalTimeoutDispatcher autoCloseTimeout;
    };
}
