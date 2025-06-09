#pragma once

#include <GUI/components/StateIconButton.h>

#include <mutex>

#include <GUI/signals/UniqueSignalTimeoutDispatcher.h>

namespace Clavis::GUI {
    class TaskIconButton : public StateIconButton {
    public:
        TaskIconButton();

        void SetIcons(const Icons::IconDefinition& defaultIcon, const Icons::IconDefinition& progressIcon, const Icons::IconDefinition& successIcon, const Icons::IconDefinition& failureIcon);
        void SetTooltips(const std::string& defaultTooltip, const std::string& progressTooltip, const std::string& successTooltip, const std::string& failureTooltip);

        void SetTask(const std::function<bool()>& newTask);
    protected:

    private:
        void PerformTask();

        UniqueSignalTimeoutDispatcher removeStateTimeoutDispatcher;
        Glib::Dispatcher setStateDispatcher;
        std::mutex mutex;
        State currentState;
        bool isActionRunning = false;

        std::function<bool()> task = [](){return true;};
    };
}