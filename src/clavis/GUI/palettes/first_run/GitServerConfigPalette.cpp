#include <iostream>
#include <GUI/palettes/first_run/GitServerConfigPalette.h>
#include <extensions/GitWrapper.h>
#include <system/Extensions.h>

#include "error/ClavisError.h"

namespace Clavis::GUI {
    GitServerConfigPalette::GitServerConfigPalette(const std::filesystem::path &repoPath) :
        DualChoicePalette(_(GIT_SERVER_CONFIG_TITLE))
    {
        set_margin(10);
        set_default_size(550, -1);

        passwordStoreLocation = repoPath;

        SetYesText(_(MISC_DONE_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));

        InitRadioButtons();
        PopulateBoxes();
        PopulateWidgets();
        SetButtonCallbacks();

        RefreshUI();

        if (std::string us; Git::TryGetUsername(us))
            gitUserNameEntry.set_text(us);
        if (std::string em; Git::TryGetEmail(em))
            gitUserEmailEntry.set_text(em);
    }


    void GitServerConfigPalette::InitRadioButtons() {
        infoTitleLabel.set_text(_(GIT_SERVER_CONFIG_CHOOSE_METHOD_LABEL));
        doNotUseGitCheckButton.set_label(_(GIT_SERVER_CONFIG_DO_NOT_USE_CHECKBUTTON));
        createANewRepoCheckButton.set_label(_(GIT_SERVER_CONFIG_CREATE_A_NEW_REPO_LABEL));
        cloneAnExistingRepoCheckButton.set_label(_(GIT_SERVER_CONFIG_CLONE_AN_EXISTING_REPO_LABEL));

        createANewRepoCheckButton.set_group(doNotUseGitCheckButton);
        cloneAnExistingRepoCheckButton.set_group(doNotUseGitCheckButton);

        doNotUseGitCheckButton.set_active(true);

        doNotUseGitCheckButton.signal_toggled().connect([this]() {
            if (doNotUseGitCheckButton.get_active())
                RefreshUI();
        });

        createANewRepoCheckButton.signal_toggled().connect([this]() {
            if (createANewRepoCheckButton.get_active())
                RefreshUI();
        });

        cloneAnExistingRepoCheckButton.signal_toggled().connect([this]() {
            if (cloneAnExistingRepoCheckButton.get_active())
                RefreshUI();
        });
    }

    void GitServerConfigPalette::PopulateWidgets() {
        append(infoTitleLabel);
        append(doNotUseGitCheckButton);
        append(createANewRepoCheckButton);
        append(cloneAnExistingRepoCheckButton);

        append(repoToolsVBox);
        append(gitUserSettingsVBox);
    }

    void GitServerConfigPalette::InitLabelsVBox(Gtk::Box &box, std::vector<std::tuple<Gtk::Label&, std::string>> labels) {
        box.set_orientation(Gtk::Orientation::VERTICAL);
        box.set_vexpand(true);
        box.set_margin_start(20);
        box.set_margin_end(20);

        for (const auto& kvp : labels) {
            auto& entry = std::get<0>(kvp);
            auto& text = std::get<1>(kvp);

            entry.set_text(text);
            entry.set_vexpand(true);
            box.append(entry);
        }
    }

    void GitServerConfigPalette::PopulateRepoToolsBox() {
        repoToolsTitleLabel.set_text("Not initialized");
        repoToolsTitleLabel.set_halign(Gtk::Align::CENTER);
        repoToolsTitleLabel.set_hexpand(true);
        repoToolsTitleLabel.set_margin_bottom(5);

        repoToolsVBox.set_orientation(Gtk::Orientation::VERTICAL);
        repoToolsVBox.set_hexpand(true);

        repoToolsHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        repoToolsHBox.set_hexpand(true);
        repoToolsHBox.append(repoToolsLabelsVBox);
        repoToolsHBox.append(repoToolsEntriesVBox);

        InitLabelsVBox(repoToolsLabelsVBox, {
            { repoToolsPathLabel, _(GIT_SERVER_CONFIG_LOCATION_LABEL)},
            { repoToolsUpstreamLabel, _(GIT_SERVER_CONFIG_GIT_UPSTREAM_LABEL)},
        });

        repoToolsEntriesVBox.set_orientation(Gtk::Orientation::VERTICAL);
        repoToolsEntriesVBox.append(repoToolsPathEntry);
        repoToolsEntriesVBox.append(repoToolsUpstreamHBox);

        repoToolsPathEntry.set_text(passwordStoreLocation.string());
        repoToolsPathEntry.set_editable(false);
        repoToolsPathEntry.set_sensitive(false);
        repoToolsPathEntry.set_hexpand(true);
        repoToolsPathEntry.set_margin_start(5);

        repoToolsUpstreamHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        repoToolsUpstreamHBox.set_hexpand(true);
        repoToolsUpstreamHBox.append(repoToolsUpstreamEntry);
        repoToolsUpstreamHBox.append(newRepoButton);
        repoToolsUpstreamHBox.append(cloneRepoButton);
        repoToolsUpstreamHBox.set_margin_top(5);

        repoToolsUpstreamEntry.set_margin_start(5);
        repoToolsUpstreamEntry.set_margin_end(5);
        repoToolsUpstreamEntry.set_hexpand(true);
        cloneRepoButton.SetIcons(Icons::Git::Pull, Icons::Actions::Refresh, Icons::Check, Icons::Cross);
        newRepoButton.SetIcons(Icons::Git::Init, Icons::Actions::Refresh, Icons::Check, Icons::Cross);

        Gtk::Separator sep(Gtk::Orientation::HORIZONTAL);
        repoToolsVBox.append(sep);
        sep.set_margin_top(10);
        sep.set_margin_bottom(10);

        repoToolsVBox.append(repoToolsTitleLabel);
        repoToolsVBox.append(repoToolsHBox);
    }


    void GitServerConfigPalette::PopulateGitUserSettingsBox() {
        gitUserSettingsLabel.set_text(_(GIT_SERVER_CONFIG_USER_SETTINGS_LABEL));
        gitUserSettingsLabel.set_halign(Gtk::Align::CENTER);
        gitUserSettingsLabel.set_hexpand(true);
        gitUserSettingsLabel.set_margin_bottom(5);

        gitUserSettingsVBox.set_hexpand(true);
        gitUserSettingsVBox.set_orientation(Gtk::Orientation::VERTICAL);

        gitUserSettingsHBox.set_hexpand(true);
        gitUserSettingsHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        gitUserSettingsHBox.append(gitUserSettingsLabelsVBox);
        gitUserSettingsHBox.append(gitUserSettingsEntriesVBox);

        InitLabelsVBox(gitUserSettingsLabelsVBox,
            {
                { gitUserNameLabel, _(GIT_SERVER_CONFIG_GIT_USERNAME_LABEL)},
                { gitUserEmailLabel, _(GIT_SERVER_CONFIG_GIT_EMAIL_LABEL)},
            }
        );

        gitUserSettingsEntriesVBox.set_orientation(Gtk::Orientation::VERTICAL);
        gitUserSettingsEntriesVBox.append(gitUserNameEntry);
        gitUserSettingsEntriesVBox.append(gitUserEmailEntry);
        gitUserNameEntry.set_hexpand(true);
        gitUserEmailEntry.set_hexpand(true);
        gitUserNameEntry.set_margin_bottom(5);


        Gtk::Separator sep(Gtk::Orientation::HORIZONTAL);
        gitUserSettingsVBox.append(sep);
        sep.set_margin_top(10);
        sep.set_margin_bottom(10);

        gitUserSettingsVBox.append(gitUserSettingsLabel);
        gitUserSettingsVBox.append(gitUserSettingsHBox);
    }

    void GitServerConfigPalette::SetButtonCallbacks() {
        newRepoButton.SetTask([this]() {
            auto upstream = repoToolsUpstreamEntry.get_text();
            if (upstream.empty() || !Git::TryInitRepo(passwordStoreLocation, upstream)) {
                repoToolsUpstreamEntry.DisplayError();
                return false;
            }

            return true;
        });

        cloneRepoButton.SetTask([this]() {
            auto upstream = repoToolsUpstreamEntry.get_text();
            if (upstream.empty() || !Git::TryCloneRepo(passwordStoreLocation, upstream)) {
                repoToolsUpstreamEntry.DisplayError();
                return false;
            }

            return true;
        });
    }



    void GitServerConfigPalette::PopulateBoxes() {
        PopulateRepoToolsBox();
        PopulateGitUserSettingsBox();
    }

    void GitServerConfigPalette::RefreshUI() {
        if (doNotUseGitCheckButton.get_active()) {
            repoToolsVBox.hide();
            gitUserSettingsVBox.hide();
        }

        if (createANewRepoCheckButton.get_active()) {
            repoToolsVBox.show();
            gitUserSettingsVBox.show();

            newRepoButton.show();
            cloneRepoButton.hide();

            repoToolsTitleLabel.set_text(_(GIT_SERVER_CONFIG_CREATE_A_NEW_REPO_LABEL) + ":");
        }

        if (cloneAnExistingRepoCheckButton.get_active()) {
            repoToolsVBox.show();
            gitUserSettingsVBox.show();

            newRepoButton.hide();
            cloneRepoButton.show();

            repoToolsTitleLabel.set_text(_(GIT_SERVER_CONFIG_CLONE_AN_EXISTING_REPO_LABEL) + ":");
        }
    }

    void GitServerConfigPalette::DoGiveResponse(bool r) {
        if (!r) {
            DualChoicePalette::DoGiveResponse(false);
            return;
        }

        if (doNotUseGitCheckButton.get_active())
            DualChoicePalette::DoGiveResponse(true);
        else if (IsRepoValid() && TryApplyGitUserSettings())
            DualChoicePalette::DoGiveResponse(true);
    }

    bool GitServerConfigPalette::IsRepoValid() {
        bool success = true;
        if (!System::DirectoryExists(passwordStoreLocation) || ! Git::IsGitRepo(passwordStoreLocation)) {
            repoToolsUpstreamEntry.DisplayError();
            success = false;
        }

        if (!IsGitUserSettingsValid())
            success = false;

        return success;
    }

    bool GitServerConfigPalette::IsGitUserSettingsValid() {
        bool success = true;
        if (gitUserNameEntry.IsEmpty()) {
           gitUserNameEntry.DisplayError();
            success = false;
        }

        if (gitUserEmailEntry.IsEmpty()) {
            gitUserEmailEntry.DisplayError();
            success = false;
        }

        return success;
    }


    bool GitServerConfigPalette::TryApplyGitUserSettings() const {
        return Git::TrySetUsername(gitUserNameEntry.get_text()) && Git::TrySetEmail(gitUserEmailEntry.get_text());
    }




}
