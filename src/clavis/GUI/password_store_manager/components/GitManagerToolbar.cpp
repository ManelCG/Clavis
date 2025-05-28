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
        gitPullButton.SetDefaultIcon(Icons::Git::Pull);
        gitPullButton.SetDefaultTooltip(_(GIT_PULL_PASSWORDS));
        gitPullButton.RegisterIcon(StateIconButton::State::PROGRESS, Icons::Actions::Refresh);
        gitPullButton.RegisterIcon(StateIconButton::State::SUCCESS, Icons::Check);
        gitPullButton.RegisterIcon(StateIconButton::State::ERROR, Icons::Cross);
        gitPullButton.RegisterTooltip(StateIconButton::State::PROGRESS, _(GIT_PULLING_PASSWORDS));
        gitPullButton.RegisterTooltip(StateIconButton::State::SUCCESS, _(GIT_PULLED_PASSWORDS));
        gitPullButton.RegisterTooltip(StateIconButton::State::ERROR, _(GIT_ERROR_PULLING_PASSWORDS));

        gitPushButton.SetDefaultIcon(Icons::Git::Push);
        gitPushButton.SetDefaultTooltip(_(GIT_PUSH_PASSWORDS));
        gitPushButton.RegisterIcon(StateIconButton::State::PROGRESS, Icons::Actions::Refresh);
        gitPushButton.RegisterIcon(StateIconButton::State::SUCCESS, Icons::Check);
        gitPushButton.RegisterIcon(StateIconButton::State::ERROR, Icons::Cross);
        gitPushButton.RegisterTooltip(StateIconButton::State::PROGRESS, _(GIT_PUSHING_PASSWORDS));
        gitPushButton.RegisterTooltip(StateIconButton::State::SUCCESS, _(GIT_PUSHED_PASSWORDS));
        gitPushButton.RegisterTooltip(StateIconButton::State::ERROR, _(GIT_ERROR_PUSHING_PASSWORDS));

        gitSyncButton.SetDefaultIcon(Icons::Git::Sync);
        gitSyncButton.SetDefaultTooltip(_(GIT_SYNC_PASSWORDS));
        gitSyncButton.RegisterIcon(StateIconButton::State::PROGRESS, Icons::Actions::Refresh);
        gitSyncButton.RegisterIcon(StateIconButton::State::SUCCESS, Icons::Check);
        gitSyncButton.RegisterIcon(StateIconButton::State::ERROR, Icons::Cross);
        gitSyncButton.RegisterTooltip(StateIconButton::State::PROGRESS, _(GIT_SYNCING_PASSWORDS));
        gitSyncButton.RegisterTooltip(StateIconButton::State::SUCCESS, _(GIT_SYNCED_PASSWORDS));
        gitSyncButton.RegisterTooltip(StateIconButton::State::ERROR, _(GIT_ERROR_SYNCING_PASSWORDS));

        gitPullButton.set_margin_end(1);
        gitPushButton.set_margin_end(1);

        append(gitPushButton);
        append(gitPullButton);
        append(gitSyncButton);

        gitPullButton.signal_clicked().connect([this]() {
            PerformGitAction(gitPullButton, Git::TryPull);
        });

        gitPushButton.signal_clicked().connect([this]() {
            PerformGitAction(gitPushButton, Git::TryPush);
        });

        gitSyncButton.signal_clicked().connect([this]() {
            PerformGitAction(gitSyncButton, Git::TrySync);
        });

        if (!Git::IsGitRepo()) {
            SetSensitive(false);
            gitPullButton.SetDefaultTooltip(_(GIT_IS_NOT_INITIALIZED_TOOLTIP));
            gitPushButton.SetDefaultTooltip(_(GIT_IS_NOT_INITIALIZED_TOOLTIP));
            gitSyncButton.SetDefaultTooltip(_(GIT_IS_NOT_INITIALIZED_TOOLTIP));
        }

    }

    void GitManagerToolbar::SetSensitive(bool sensitive) {
        gitPullButton.set_sensitive(sensitive);
        gitPushButton.set_sensitive(sensitive);
        gitSyncButton.set_sensitive(sensitive);
    }

    void GitManagerToolbar::RemoveState() {
        gitPullButton.RemoveState();
        gitPushButton.RemoveState();
        gitSyncButton.RemoveState();
    }

    void GitManagerToolbar::SetOnSync(std::function<void()> action) {
        onSync = action;
    }

    void GitManagerToolbar::PerformGitAction(StateIconButton& button, std::function<bool()> action){
        if (isGitActionRunning)
            return;

        // This is just needed because the 3s delay to remove style can act janky
        // So we make it remember "who is the last thread that set its style" kinda
        int styleSetterID = Extensions::RNG::GetInt32();

        // Disallow more git actions to avoid corruption or something
        isGitActionRunning = true;

        // Disallow pressing other buttons. Also remove their style.
        SetSensitive(false);
        RemoveState();

        // Our button stays sensitive because otherwise it loses its color
        button.set_sensitive(true);
        button.ApplyState(StateIconButton::State::PROGRESS);

        button.RegisterSetterID(styleSetterID);

        std::thread t([this, &button, action, styleSetterID]() {
            // Start the git action
            const auto success = action();

            if (success)
                button.ApplyState(StateIconButton::State::SUCCESS);
            else
                button.ApplyState(StateIconButton::State::ERROR);

            // Allow other buttons to be pressed again
            SetSensitive(true);
            isGitActionRunning = false;

            // This should refresh the folderView, but they must set their own lambda for it
            onSync();

            // Remove success/error style after 3 seconds.
            Glib::signal_timeout().connect_once(
                [this, &button, styleSetterID]() {
                    if (button.GetSetterID() == styleSetterID)
                        button.RemoveState();
                },
                3000
            );

        });
        t.detach();
    }

}
