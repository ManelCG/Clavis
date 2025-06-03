#pragma once

#include <gtkmm.h>

#include <GUI/password_store_manager/PasswordStoreManager.h>

namespace Clavis::GUI {
    class Workflows {
    public:
        static void NewPasswordWorkflow(PasswordStoreManager* passwordStoreManager);
        static void NewFolderWorkflow(PasswordStoreManager* passwordStoreManager);

        static void DeleteElementWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void RenameElementWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
        static void EditPasswordWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);

    private:
        static void NewPasswordWorkflow_IMPL(PasswordStoreManager* passwordStoreManager, const std::string& name = "");
    };

}
