#pragma once

#include <gtkmm.h>

#include <GUI/components/IconButton.h>
#include <GUI/password_store_manager/components/GitManagerToolbar.h>

namespace Clavis::GUI {
    class PasswordStoreManagerTools : public Gtk::Box {
    public:
        PasswordStoreManagerTools();

        void SetOnNewFolderButtonClick(const std::function<void()> &callback);
        void SetOnGoUpButtonClick(const std::function<void()> &callback);
        void SetOnNewPasswordButtonClick(const std::function<void()> &callback);
        void SetOnNewTwoFactorButtonClick(const std::function<void()> &callback);
        void SetOnNewWorkspaceButtonClick(const std::function<void()> &callback);
        void SetOnRefreshButtonClick(const std::function<void()> &callback);

        void SetOnGitSync(const std::function<void()> &callback);

        void SetGoUpButtonActive(bool active);

        // Everything these buttons create lands in a real directory. Inside a workspace there is
        // no such directory in view, so they are greyed out rather than quietly creating items in
        // whatever folder the workspace happens to live in.
        void SetCreationButtonsActive(bool active);

        // Make sure to pass a relative path here, or it will be very ugly.
        void SetPath(const std::filesystem::path& path);

        // For places that are not a directory at all, such as a workspace, which reads better as
        // its own name than as the folder it happens to be filed under.
        void SetPathLabel(const std::string& text);

        void PerformGitAction(GitManagerToolbar::Action action);

    protected:

    private:
        IconButton newFolderButton;
        IconButton goUpButton;
        IconButton newPasswordButton;
        IconButton newTwoFactorButton;
        IconButton newWorkspaceButton;
        IconButton refreshButton;

        Gtk::ScrolledWindow labelScrollBox;
        Gtk::Label pathLabel;

        Gtk::Box toolHBox;

        GitManagerToolbar gitToolbar;

        std::function<void()> onNewFolderButtonClick;
        std::function<void()> onGoUpButtonClick;
        std::function<void()> onNewPasswordButtonClick;
        std::function<void()> onNewTwoFactorButtonClick;
        std::function<void()> onNewWorkspaceButtonClick;
        std::function<void()> onRefreshButtonClick;

        std::function<void()> onGitSync;
    };
}