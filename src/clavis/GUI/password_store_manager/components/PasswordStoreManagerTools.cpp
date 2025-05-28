#include <GUI/password_store_manager/components/PasswordStoreManagerTools.h>

namespace Clavis::GUI {
    PasswordStoreManagerTools::PasswordStoreManagerTools() :
        Gtk::Box(Gtk::Orientation::VERTICAL),

        newFolderButton(Icons::Actions::NewFolder),
        goUpButton(Icons::UpArrow),
        newPasswordButton(Icons::Plus),
        refreshButton(Icons::Actions::Refresh)
    {
        toolHBox.append(newFolderButton);
        toolHBox.append(goUpButton);
        toolHBox.append(newPasswordButton);
        toolHBox.append(refreshButton);

        labelScrollBox.set_child(pathLabel);
        labelScrollBox.set_hexpand(true);
        labelScrollBox.set_vexpand(false);
        labelScrollBox.set_margin_top(5);
        labelScrollBox.set_margin_bottom(0);
        labelScrollBox.set_margin_start(5);
        labelScrollBox.set_margin_end(15);
        labelScrollBox.set_min_content_height(0);
        labelScrollBox.property_vscrollbar_policy().set_value(Gtk::PolicyType::NEVER);

        toolHBox.append(gitToolbar);
        gitToolbar.set_halign(Gtk::Align::END);
        gitToolbar.set_hexpand(true);
        gitToolbar.set_margin_end(10);

        newFolderButton.set_margin_end(1);
        goUpButton.set_margin_end(1);
        newPasswordButton.set_margin_end(1);

        append(toolHBox);
        append(labelScrollBox);
    }

    void PasswordStoreManagerTools::SetPath(const std::filesystem::path &path) {
        if (const auto str = path.string(); str != ".")
            pathLabel.set_text(str);
        else
            pathLabel.set_text("");
    }


    void PasswordStoreManagerTools::SetGoUpButtonActive(bool active) {
        goUpButton.set_sensitive(active);
    }


    void PasswordStoreManagerTools::SetOnNewFolderButtonClick(const std::function<void()> &callback) {
        onNewFolderButtonClick = callback;
        newFolderButton.signal_clicked().connect(onNewFolderButtonClick);
    }
    void PasswordStoreManagerTools::SetOnGoUpButtonClick(const std::function<void()> &callback) {
        onGoUpButtonClick =  callback;
        goUpButton.signal_clicked().connect(onGoUpButtonClick);
    }
    void PasswordStoreManagerTools::SetOnNewPasswordButtonClick(const std::function<void()> &callback) {
        onNewPasswordButtonClick = callback;
        newPasswordButton.signal_clicked().connect(onNewPasswordButtonClick);
    }
    void PasswordStoreManagerTools::SetOnRefreshButtonClick(const std::function<void()> &callback) {
        onRefreshButtonClick = callback;
        refreshButton.signal_clicked().connect(onRefreshButtonClick);
    }
    void PasswordStoreManagerTools::SetOnGitSync(const std::function<void()> &callback) {
        gitToolbar.SetOnSync(callback);
    }



}
