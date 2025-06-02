#include <GUI/password_store_manager/components/GitManagerToolbar.h>

#include <thread>

#include <extensions/GitWrapper.h>
#include <extensions/RNG.h>
#include <language/Language.h>

#include "settings/SettingsDefinitions.h"


namespace Clavis::GUI {
    GitManagerToolbar::GitManagerToolbar() :
        Gtk::Box(Gtk::Orientation::HORIZONTAL)
    {
        InitializeButton(
            Icons::Git::Pull,
            _(GIT_PULL_PASSWORDS),
            _(GIT_PULLING_PASSWORDS),
            _(GIT_PULLED_PASSWORDS),
            _(GIT_ERROR_PULLING_PASSWORDS),
            Git::TryPull
        );

        InitializeButton(
            Icons::Git::Push,
            _(GIT_PUSH_PASSWORDS),
            _(GIT_PUSHING_PASSWORDS),
            _(GIT_PUSHED_PASSWORDS),
            _(GIT_ERROR_PUSHING_PASSWORDS),
            Git::TryPush
        );

        InitializeButton(
            Icons::Git::Sync,
            _(GIT_SYNC_PASSWORDS),
            _(GIT_SYNCING_PASSWORDS),
            _(GIT_SYNCED_PASSWORDS),
            _(GIT_ERROR_SYNCING_PASSWORDS),
            Git::TrySync
        );

        // gitPullButton.set_margin_end(1);
        // gitPushButton.set_margin_end(1);

        if (!Git::IsGitRepo())
            SetSensitive(false);

        buttonStyleDispatcher.connect([this]() {
            std::lock_guard lock(mutex);

            const auto id = buttonIDApplyStypeThreadSafe;
            const auto state = buttonStateThreadSafe;

            auto button = buttons[id];
            button->ApplyState(state);
        });

        removeStyleDispatcher.connect([this]() {
            std::lock_guard lock(mutex);
            auto buttonID = buttonIDRemoveStyleThreadSafe;

            buttons[buttonID]->RemoveState();
        });

        sensitiveDispatcher.connect([this]() {
            std::lock_guard lock(mutex);
            auto sensitive = sensitiveThreadSafe;

            SetSensitive(sensitive);
        });

    }

    void GitManagerToolbar::SetSensitive(bool sensitive) {
        for (const auto& kvp : buttons)
            kvp.second->set_sensitive(sensitive);
    }

    void GitManagerToolbar::RemoveState() {
        for (const auto& kvp : buttons)
            kvp.second->RemoveState();
    }

    void GitManagerToolbar::SetOnSync(const std::function<void()> &action) {
        onSync = action;
    }

    void GitManagerToolbar::SetStateThreadsafe(int button, StateIconButton::State state) {
        std::lock_guard lock(mutex);

        buttonIDApplyStypeThreadSafe = button;
        buttonStateThreadSafe = state;

        buttonStyleDispatcher.emit();
    }

    void GitManagerToolbar::SetSensitiveThreadsafe(bool sensitive) {
        std::lock_guard lock(mutex);
        sensitiveThreadSafe = sensitive;
        sensitiveDispatcher.emit();
    }

    void GitManagerToolbar::RemoveStateThreadsafe(int button) {
        std::lock_guard lock(mutex);
        buttonIDRemoveStyleThreadSafe = button;
        removeStyleDispatcher.emit();
    }


    void GitManagerToolbar::InitializeButton(const Icons::IconDefinition &defaultIcon, const std::string &defaultTooltip, const std::string &textProgress, const std::string &textSuccess, const std::string &textError, std::function<bool()> action) {
        auto button = Gtk::make_managed<StateIconButton>();
        auto buttonID = static_cast<int>(buttons.size());

        button->set_name(std::to_string(buttonID));
        buttons.insert({buttonID, button});

        button->SetDefaultIcon(defaultIcon);
        button->SetDefaultTooltip(defaultTooltip);

        button->RegisterIcon(StateIconButton::State::PROGRESS, Icons::Actions::Refresh);
        button->RegisterIcon(StateIconButton::State::SUCCESS, Icons::Check);
        button->RegisterIcon(StateIconButton::State::ERROR, Icons::Cross);

        button->RegisterTooltip(StateIconButton::State::PROGRESS, textProgress);
        button->RegisterTooltip(StateIconButton::State::SUCCESS, textSuccess);
        button->RegisterTooltip(StateIconButton::State::ERROR, textError);

        button->set_margin_start(1);

        button->signal_clicked().connect([this, action, button]() {
            PerformGitAction(button, action);
        });

        if (!Git::IsGitRepo())
            button->SetDefaultTooltip(_(GIT_IS_NOT_INITIALIZED_TOOLTIP));

        append(*button);
    }


    void GitManagerToolbar::PerformGitAction(StateIconButton* button, std::function<bool()> action){
        // Disallow more git actions to avoid corruption or something
        {
            std::lock_guard lock(mutex);
            if (isGitActionRunning)
                return;

            isGitActionRunning = true;
        }

        // This is just needed because the 3s delay to remove style can act janky
        // So we make it remember "who is the last thread that set its style" kinda
        int styleSetterID = Extensions::RNG::GetInt32();
        int buttonID = std::stoi(button->get_name());

        // Disallow pressing other buttons. Also remove their style.
        SetSensitive(false);
        RemoveState();

        // Our button stays sensitive because otherwise it loses its color
        button->set_sensitive(true);
        button->ApplyState(StateIconButton::State::PROGRESS);

        button->RegisterSetterID(styleSetterID);

        std::thread t([this, button, action, styleSetterID, buttonID]() {
            // Start the git action
            const auto success = action();

            if (success)
                SetStateThreadsafe(buttonID, StateIconButton::State::SUCCESS);
            else
                SetStateThreadsafe(buttonID, StateIconButton::State::ERROR);

            // Allow other buttons to be pressed again
            SetSensitiveThreadsafe(true);

            {
                std::lock_guard lock(mutex);
                isGitActionRunning = false;
            }


            // This should refresh the folderView, but they must set their own lambda for it
            onSync();

            // Remove success/error style after 3 seconds.
            Glib::signal_timeout().connect_once(
                [this, button, styleSetterID, buttonID]() {
                    if (button->GetSetterID() == styleSetterID)
                        RemoveStateThreadsafe(buttonID);
                },
                3000
            );
        });
        t.detach();
    }

}
