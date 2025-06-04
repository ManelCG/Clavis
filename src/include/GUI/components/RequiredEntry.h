#pragma once

#include <gtkmm.h>
#include <GUI/signals/UniqueSignalTimeoutDispatcher.h>

namespace Clavis::GUI {
    class RequiredEntry : public Gtk::Entry {
    public:
        RequiredEntry();
        void DisplayError();

        bool IsValid();
        void SetCorrectnessCheck(std::function<bool()> lambda);

    protected:

    private:
        UniqueSignalTimeoutDispatcher timeoutDispatcher;
        std::function<bool()> isTextValid;
    };
}