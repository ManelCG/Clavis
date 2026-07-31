#pragma once

#include <memory>
#include <string>
#include <vector>

#include <gtkmm.h>

#include <two_factor/TwoFactorEntry.h>

#include <GUI/components/IconButton.h>
#include <GUI/palettes/Palette.h>
#include <GUI/signals/UniqueSignalTimeoutDispatcher.h>

namespace Clavis::GUI {
    // Read-only view of everything stored alongside the code itself: the OTP parameters, the
    // recovery codes and any notes. This is how recovery codes get back out of the store.
    //
    // The shared secret is deliberately not shown here -- revealing it is what "Transfer 2FA" is
    // for, and that window carries the corresponding warning.
    class TwoFactorDetailsPalette : public Palette {
    public:
        TwoFactorDetailsPalette(const std::string& name, const TwoFactor::TwoFactorEntry& entry);

    protected:

    private:
        void AddParameterRow(int row, const Glib::ustring& label, const std::string& value);
        void AddRecoveryRow(size_t oneBasedIndex, const std::string& code);

        TwoFactor::TwoFactorEntry entry;

        Gtk::Box mainVBox;
        Gtk::Grid parameterGrid;

        Gtk::Label recoveryHeaderLabel;
        Gtk::Box recoveryVBox;
        Gtk::ScrolledWindow recoveryScroll;

        Gtk::Label notesHeaderLabel;
        Gtk::Label notesLabel;

        // Rows are built dynamically, so their widgets have to be owned somewhere.
        std::vector<std::unique_ptr<Gtk::Widget>> ownedWidgets;

        UniqueSignalTimeoutDispatcher clipboardClearTimeout;
    };
}
