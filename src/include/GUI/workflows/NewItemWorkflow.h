#pragma once

#include <gtkmm.h>

#include <GUI/password_store_manager/PasswordStoreManager.h>

namespace Clavis::GUI::Workflows {
    void NewPasswordWorkflow(PasswordStoreManager* passwordStoreManager);
    void NewFolderWorkflow(PasswordStoreManager* passwordStoreManager);

    void DeleteElementWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
    void RenameElementWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);
    void EditPasswordWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& element);

}
