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
        void SetOnRefreshButtonClick(const std::function<void()> &callback);

        void SetOnGitSync(const std::function<void()> &callback);

        void SetGoUpButtonActive(bool active);

        // Make sure to pass a relative path here, or it will be very ugly.
        void SetPath(const std::filesystem::path& path);

    protected:

    private:
        IconButton newFolderButton;
        IconButton goUpButton;
        IconButton newPasswordButton;
        IconButton refreshButton;

        Gtk::ScrolledWindow labelScrollBox;
        Gtk::Label pathLabel;

        Gtk::Box toolHBox;

        GitManagerToolbar gitToolbar;

        std::function<void()> onNewFolderButtonClick;
        std::function<void()> onGoUpButtonClick;
        std::function<void()> onNewPasswordButtonClick;
        std::function<void()> onRefreshButtonClick;

        std::function<void()> onGitSync;
    };
}