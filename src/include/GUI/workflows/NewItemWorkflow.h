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

        static void NewWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager);
        static void EditWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void RenameWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void DeleteWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);

        static void AddToWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void RemoveFromWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void RenameInWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);

        // A workspace entry whose file is gone: the only thing left to offer is taking it out.
        static void MissingWorkspaceElementWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);

        // Generates the next HOTP code: re-reads the file, advances the counter, re-encrypts
        // and commits. Never a side effect of merely selecting or copying an entry.
        static void AdvanceHotpCounterWorkflow(PasswordStoreManager* passwordStoreManager, const std::filesystem::path& fullpath);

    private:
        // Writes the workspaces database back to disk under a message describing what changed,
        // reporting rather than swallowing a failure.
        static void SaveWorkspaces(PasswordStoreManager* passwordStoreManager, const std::string& commitMessage);

        // Identifies the workspace an element stands for, from the virtual path it carries.
        static bool TryResolveWorkspace(PasswordStoreManager* passwordStoreManager,
                                        const PasswordStoreElements::PasswordStoreElement& element,
                                        std::filesystem::path& outDir, std::string& outName);

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
