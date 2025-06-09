#include <GUI/components/TaskIconButton.h>

#include <thread>

namespace Clavis::GUI {
    TaskIconButton::TaskIconButton() {
        signal_clicked().connect([this]() {
            PerformTask();
        });

        setStateDispatcher.connect([this]() {
            std::lock_guard lock(mutex);
            ApplyState(currentState);
        });

        removeStateTimeoutDispatcher.SetAction([this]() {
            std::lock_guard lock(mutex);
            RemoveState();
        });

        removeStateTimeoutDispatcher.SetSeconds(3);
    }
    void TaskIconButton::SetIcons(const Icons::IconDefinition &defaultIcon, const Icons::IconDefinition &progressIcon, const Icons::IconDefinition &successIcon, const Icons::IconDefinition &failureIcon) {
        SetDefaultIcon(defaultIcon);

        RegisterIcon(State::PROGRESS, progressIcon);
        RegisterIcon(State::SUCCESS, successIcon);
        RegisterIcon(State::ERROR, failureIcon);
    }

    void TaskIconButton::SetTooltips(const std::string &defaultTooltip, const std::string &progressTooltip, const std::string &successTooltip, const std::string &failureTooltip) {
        SetDefaultTooltip(defaultTooltip);

        RegisterTooltip(State::PROGRESS, progressTooltip);
        RegisterTooltip(State::SUCCESS, successTooltip);
        RegisterTooltip(State::ERROR, failureTooltip);
    }

    void TaskIconButton::SetTask(const std::function<bool()> &newTask) {
        task = newTask;
    }

    void TaskIconButton::PerformTask() {
        std::lock_guard lock(mutex);
        if (isActionRunning)
            return;

        isActionRunning = true;

        ApplyState(State::PROGRESS);

        std::thread t([this]() {
            const auto success = task();

            std::lock_guard lockT(mutex);

            if (success)
                currentState = State::SUCCESS;
            else
                currentState = State::ERROR;

            setStateDispatcher.emit();

            isActionRunning = false;
            removeStateTimeoutDispatcher.StartTimeout();
        });
        t.detach();
    }

}