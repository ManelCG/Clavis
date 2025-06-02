#pragma once

#include <mutex>

#include <gtkmm.h>

#include <GUI/components/StateIconButton.h>
#include <GUI/signals/UniqueSignalTimeout.h>

namespace Clavis::GUI {
    class GitManagerToolbar : public Gtk::Box {
    public:
        enum class Action {
            Push = 1,
            Pull = 2,
            Sync = 3
        };

        GitManagerToolbar();

        void PerformGitAction(Action gitAction);
        void SetOnSync(const std::function<void()> &action);

    protected:

    private:
        void InitializeButton(Action gitAction, const Icons::IconDefinition& defaultIcon, const std::string& defaultTooltip, const std::string& textProgress, const std::string& textSuccess, const std::string& textError, std::function<bool()> action);

        void SetSensitive(bool sensitive);
        void RemoveState();

        void SetStateThreadsafe(Action button, StateIconButton::State state);
        void RemoveStateThreadsafe(Action button);
        void SetSensitiveThreadsafe(bool sensitive);

        Glib::Dispatcher buttonStyleDispatcher;
        Glib::Dispatcher removeStyleDispatcher;
        Glib::Dispatcher sensitiveDispatcher;

        UniqueSignalTimeout uniqueSignalTimeout;

        std::mutex mutex;
        Action buttonIDApplyStypeThreadSafe;
        Action buttonIDRemoveStyleThreadSafe;
        StateIconButton::State buttonStateThreadSafe;
        bool sensitiveThreadSafe;

        std::map<Action, StateIconButton*> buttons;
        std::map<Action, std::function<bool()>> gitActions;

        bool isGitActionRunning = false;

        std::function<void()> onSync = [](){};
    };
}