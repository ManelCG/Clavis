#pragma once

#include <gtkmm.h>

#include <GUI/password_store_manager/PasswordStoreManager.h>

namespace Clavis::GUI {
    enum class FileOpenDialogAction {
        OPEN_FILE,
        SAVE_FILE,
        OPEN_FOLDER,
    };

    class Workflows {
    public:
        static void NewPasswordWorkflow(PasswordStoreManager* passwordStoreManager);
        static void NewFolderWorkflow(PasswordStoreManager* passwordStoreManager);

        static void DeleteElementWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void RenameElementWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void EditPasswordWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);

        static void ConfigGPGKeyWorkflow(PasswordStoreManager* passwordStoreManager);

        static bool ExportGPGWorkflow(const std::string& gpgid, Gtk::Window* parent = nullptr);
        static bool ImportGPGWorkflow(Gtk::Window* parent, std::string& outFingerprint);
        static bool CreateGPGWorkflow(Gtk::Window* parent, std::string& outFingerprint);

        static bool OpenFileDialog(FileOpenDialogAction action, std::string& outSelectedPath, Gtk::Widget* parent = nullptr);

        static void FirstRunWorkflow(const Glib::RefPtr<Gtk::Application> &app);

    private:
        static void NewPasswordWorkflow_IMPL(PasswordStoreManager* passwordStoreManager, const std::string& name = "");
    };

}
