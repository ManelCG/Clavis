#pragma once

#include <mutex>

#include <gtkmm.h>

#include <GUI/components/StateIconButton.h>

namespace Clavis::GUI {
    class GitManagerToolbar : public Gtk::Box {
    public:
        GitManagerToolbar();

        void SetOnSync(const std::function<void()> &action);

    protected:

    private:
        void InitializeButton(const Icons::IconDefinition& defaultIcon, const std::string& defaultTooltip, const std::string& textProgress, const std::string& textSuccess, const std::string& textError, std::function<bool()> action);

        void PerformGitAction(StateIconButton* button, std::function<bool()> action);
        void SetSensitive(bool sensitive);
        void RemoveState();

        void SetStateThreadsafe(int button, StateIconButton::State state);
        void SetSensitiveThreadsafe(bool sensitive);
        void RemoveStateThreadsafe(int button);

        Glib::Dispatcher buttonStyleDispatcher;
        Glib::Dispatcher removeStyleDispatcher;
        Glib::Dispatcher sensitiveDispatcher;

        std::mutex mutex;
        int buttonIDApplyStypeThreadSafe;
        int buttonIDRemoveStyleThreadSafe;
        StateIconButton::State buttonStateThreadSafe;
        bool sensitiveThreadSafe;

        std::map<int, StateIconButton*> buttons;

        bool isGitActionRunning = false;

        std::function<void()> onSync = [](){};
    };
}