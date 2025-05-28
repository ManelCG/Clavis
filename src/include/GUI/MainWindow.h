#pragma once

#include <gtkmm.h>

#include <GUI/menubar/MenuBar.h>

#include <GUI/password_store_manager/PasswordStoreManager.h>
#include <GUI/Titlebar/Titlebar.h>

namespace Clavis::GUI {
    class MainWindow : public Gtk::Window {
    public:
        MainWindow();

    protected:

    private:
        void InitMenuBar();

        void setCustomTitlebar();

        void TryPushPasswords();
        void TryPullPasswords();
        void TrySyncPasswords();



        int Width;
        int Height;

        std::shared_ptr<Titlebar> windowTitlebar;

        Gtk::Box mainVBox;

        MenuBar::MenuBar mainWindowMenuBar;

        PasswordStoreManager passwordStoreManager;
    };

}
