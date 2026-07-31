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
        static bool ExportGPGKeyDirectWorkflow(bool exportPrivate, Gtk::Window* parent = nullptr);
        static bool ImportGPGWorkflow(Gtk::Window* parent, std::string& outFingerprint);
        static bool CreateGPGWorkflow(Gtk::Window* parent, std::string& outFingerprint);

        static bool OpenFileDialog(FileOpenDialogAction action, std::string& outSelectedPath, Gtk::Widget* parent = nullptr);

        static bool FirstRunWorkflow(const Glib::RefPtr<Gtk::Application> &app);

        static void ExportPasswordStoreWorkflow(PasswordStoreManager* passwordStoreManager, Gtk::Window* parent);
        static void ExportFolderWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& folder);
        static void ImportPasswordStoreWorkflow(PasswordStoreManager* passwordStoreManager, Gtk::Window* parent);

        static void NewTwoFactorWorkflow(PasswordStoreManager* passwordStoreManager);
        static void EditTwoFactorWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void ShowTwoFactorDetailsWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void TransferTwoFactorWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void ImportTwoFactorMigrationWorkflow(PasswordStoreManager* passwordStoreManager, const std::string& initialUri = "");

        // Generates the next HOTP code: re-reads the file, advances the counter, re-encrypts
        // and commits. Never a side effect of merely selecting or copying an entry.
        static void AdvanceHotpCounterWorkflow(PasswordStoreManager* passwordStoreManager, const std::filesystem::path& fullpath);

    private:
        static void NewPasswordWorkflow_IMPL(PasswordStoreManager* passwordStoreManager, const std::string& name = "", const std::filesystem::path& elementPath = {});

        static void NewTwoFactorWorkflow_IMPL(PasswordStoreManager* passwordStoreManager,
                                              const std::string& defaultName,
                                              const TwoFactor::TwoFactorEntry& initial);

        static bool TryLoadTwoFactor(PasswordStoreManager* passwordStoreManager,
                                     const PasswordStoreElements::PasswordStoreElement& element,
                                     TwoFactor::TwoFactorEntry& out);

        static bool SaveTwoFactor(PasswordStoreManager* passwordStoreManager,
                                  const std::filesystem::path& fullpath,
                                  const TwoFactor::TwoFactorEntry& entry,
                                  const std::string& name,
                                  bool isEditing);
    };

}
