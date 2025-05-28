#pragma once

#include <gtkmm.h>

#include <GUI/components/StateIconButton.h>

namespace Clavis::GUI {
    class GitManagerToolbar : public Gtk::Box {
    public:
        GitManagerToolbar();

        void SetOnSync(std::function<void()> action);

    protected:

    private:
        void PerformGitAction(StateIconButton& button, std::function<bool()> action);
        void SetSensitive(bool sensitive);
        void RemoveState();

        StateIconButton gitPullButton;
        StateIconButton gitPushButton;
        StateIconButton gitSyncButton;

        bool isGitActionRunning = false;

        std::function<void()> onSync = [](){};
    };
}