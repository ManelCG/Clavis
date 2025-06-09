#include <iostream>
#include <GUI/MainWindow.h>

#include <language/Language.h>
#include <GUI/palettes/ExceptionPalette.h>
#include <extensions/GitWrapper.h>
#include <extensions/GUIExtensions.h>

#include <GUI/palettes/AboutPalette.h>
#include <GUI/palettes/HelpPalette.h>
#include <GUI/palettes/PasswordStoreDataPalette.h>

#include <GUI/workflows/NewItemWorkflow.h>
#include <GUI/palettes/first_run/GitServerConfigPalette.h>

namespace Clavis::GUI {
    MainWindow::MainWindow() :
        mainVBox(Gtk::Orientation::VERTICAL)
    {
        Glib::add_exception_handler([this]() {
            auto data = Error::ClavisException::GetLastException();

            auto p = Gtk::make_managed<ExceptionPalette>(data);
            p->set_transient_for(*this);
            p->show();
        });

        Width = 360;
        Height = 650;

        set_title("Clavis");
        set_default_size(Width, Height);

        if (Settings::WINDOW_DECORATIONS.GetValue() == Settings::WINDOW_DECORATIONS.CLAVIS_CSD)
            setCustomTitlebar();

        InitMenuBar();

        passwordStoreManager.Initialize();

        mainVBox.append(*mainWindowMenuBar);
        mainVBox.append(passwordStoreManager);

        set_child(mainVBox);
    }


    void MainWindow::setCustomTitlebar() {
        windowTitlebar = std::make_shared<Titlebar>("Clavis", true, true, true);

//#ifdef __WINDOWS__
//        signal_show().connect([this]() {
//            SetupWindowsAPI();
//        });
//#endif

        windowTitlebar->LinkWindow(this);

        set_titlebar(*windowTitlebar);
    }


    void MainWindow::InitMenuBar() {
        auto def = MenuBar::MakeMenuBarDefinition("menubar", "clavis",
        {
            MenuBar::MenuSubmenu(_(MAINMENU_FILE_MENU), {
                MenuBar::MenuSection({
                    MenuBar::MenuItem(_(MAINMENU_FILE_MENU_PASSWORD_STORE_DATA), "password_store_data"),
                }),
                MenuBar::MenuSection({
                    MenuBar::MenuItem(_(MAINMENU_FILE_MENU_NEW_FOLDER), "new_folder"),
                    MenuBar::MenuItem(_(MAINMENU_FILE_MENU_NEW_PASSWORD),  "new_password"),
                }),
                MenuBar::MenuSection({
                    MenuBar::MenuItem(_(MAINMENU_FILE_MENU_IMPORT_PASSWORD_STORE), "import_password_store"),
                    MenuBar::MenuItem(_(MAINMENU_FILE_MENU_EXPORT_PASSWORD_STORE), "export_password_store"),
                }),
                MenuBar::MenuSection({
                    MenuBar::MenuItem(_(MAINMENU_FILE_MENU_EXPORT_PUBLIC_GPG),  "export_public_gpg"),
                    MenuBar::MenuItem(_(MAINMENU_FILE_MENU_EXPORT_PRIVATE_GPG), "export_private_gpg")
                }),
                MenuBar::MenuSection({
                    MenuBar::MenuItem(_(MAINMENU_FILE_MENU_QUIT), "quit"),
                })
            }),
            MenuBar::MenuSubmenu(_(MAINMENU_EDIT_MENU), {
                MenuBar::MenuSection({
                    MenuBar::MenuItem(_(GIT_PULL_PASSWORDS), "pull"),
                    MenuBar::MenuItem(_(GIT_PUSH_PASSWORDS), "push"),
                    MenuBar::MenuItem(_(GIT_SYNC_PASSWORDS), "sync"),
                }),
                MenuBar::MenuSection({
                MenuBar::MenuItem(_(MAINMENU_EDIT_MENU_GIT_SERVER_SETTINGS), "git_server_config"),
                MenuBar::MenuItem(_(MAINMENU_EDIT_MENU_GPG_KEY_SETTINGS), "gpg_key_settings"),
                }),
            }),
            MenuBar::MenuSubmenu(_(MAINMENU_HELP_MENU), {
                MenuBar::MenuSection({
                    MenuBar::MenuItem(_(MAINMENU_HELP_MENU_HELP), "help"),
                    MenuBar::MenuItem(_(MAINMENU_HELP_MENU_ABOUT), "about"),
                }),
            }),
        });

        // FILE
        def->AddAction("password_store_data", [this]() {Extensions::SpawnWindow<PasswordStoreDataPalette>([this](){return new PasswordStoreDataPalette(passwordStoreManager.GetPasswordStore());}, this);});
        def->AddAction("new_folder", [this]() {Workflows::NewFolderWorkflow(&passwordStoreManager);});
        def->AddAction("new_password", [this]() {Workflows::NewPasswordWorkflow(&passwordStoreManager);});
        def->AddAction("quit", [this](){close();});

        // EDIT
        def->AddAction("push", [this](){TryPushPasswords();});
        def->AddAction("pull", [this](){TryPullPasswords();});
        def->AddAction("sync", [this](){TrySyncPasswords();});

        def->AddAction("git_server_config", [this](){GitServerSettings();});
        def->AddAction("gpg_key_settings", [this](){GPGKeySettings();});

        // HELP
        def->AddAction("help", [this](){Extensions::SpawnWindow<HelpPalette>(this);});
        def->AddAction("about", [this](){Extensions::SpawnWindow<AboutPalette>(this);});

        mainWindowMenuBar = def->BuildMenubar(*this);
    }

    void MainWindow::TryPullPasswords() {
        passwordStoreManager.PerformGitAction(GitManagerToolbar::Action::Pull);
    }

    void MainWindow::TryPushPasswords() {
        passwordStoreManager.PerformGitAction(GitManagerToolbar::Action::Push);
    }

    void MainWindow::TrySyncPasswords() {
        passwordStoreManager.PerformGitAction(GitManagerToolbar::Action::Sync);
    }

    void MainWindow::GitServerSettings() {
        GitServerConfigPalette::Spawn(
           this,
           []() {
               return new GitServerConfigPalette(Settings::PASSWORD_STORE_PATH.GetValue());
           });
    }

    void MainWindow::GPGKeySettings() {
        Workflows::ConfigGPGKeyWorkflow(&passwordStoreManager);
    }




}
