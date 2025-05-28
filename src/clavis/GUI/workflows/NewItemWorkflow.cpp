#include <GUI/workflows/NewItemWorkflow.h>

#include <error/ClavisError.h>
#include <extensions/GPGWrapper.h>
#include <extensions/GitWrapper.h>
#include <system/Extensions.h>

#include <GUI/palettes/SimpleEntryPalette.h>
#include <GUI/palettes/NewPasswordPalette.h>
#include <GUI/palettes/SimpleYesNoQuestionPalette.h>


namespace Clavis::GUI::Workflows {
    void NewFolderWorkflow(PasswordStoreManager *passwordStoreManager) {
        auto palette = SimpleEntryPalette::Create(passwordStoreManager);
        palette->SetTitle(_(NEW_FOLDER_PALETTE_TITLE));
        palette->SetLabelText(_(NEW_FOLDER_PALETTE_LABEL_TITLE));
        palette->SetYesSuggested();

        std::string folderName = "";
        if (!palette->Run([&folderName](SimpleEntryPalette *p, bool r) {
            if (r)
                folderName = p->GetEntryText();
        }))
            return;

        auto passwordStore = passwordStoreManager->GetPasswordStore();
        auto fullpath = passwordStore.GetPath() / folderName;

        if (System::DirectoryExists(fullpath))
            RaiseClavisError(_(ERROR_DIRECTORY_ALREADY_EXISTS, folderName));

        if (!System::mkdir_p(fullpath))
            RaiseClavisError(_(ERROR_UNABLE_TO_CREATE_DIRECTORY, folderName));

        // Don't commit empty folders
        passwordStoreManager->Refresh();
    }

    void NewPasswordWorkflow(PasswordStoreManager *passwordStoreManager) {
        Password password;
        std::string name = "";
        if (!NewPasswordPalette::Spawn(passwordStoreManager, [&password, &name](NewPasswordPalette *p, bool r) {
            if (r) {
                password = p->GetPassword();
                name = p->GetPasswordName();
            }
        }))
            return;

        auto passwordStore = passwordStoreManager->GetPasswordStore();

        auto filename = name + ".gpg";
        auto fullpath = passwordStore.GetPath() / filename;

        if (passwordStore.DoesPasswordExist(filename))
            RaiseClavisError("ALREADY EXISTS. TODO: ASK FOR CONFIRMATION OF OVERWRITE");

        if (!password.TrySaveEncrypted(fullpath))
            RaiseClavisError(_(ERROR_SAVING_PASSWORD, name));

        if (Git::IsGitRepo())
            Git::CommitNewFile(fullpath, name);

        passwordStoreManager->Refresh();

    }

    void DeleteElementWorkflow(PasswordStoreManager *passwordStoreManager, const PasswordStoreElements::PasswordStoreElement &element) {
        auto palette = SimpleYesNoQuestionPalette::Create(passwordStoreManager);

        auto name = element.GetName();
        auto fullpath = element.GetPath();

        auto title = _(MISC_DELETE_ELEMENT_PROMPT, element.GetName());

        palette->SetTitle(title);
        palette->SetYesDestructive();
        palette->SetYesText(_(MISC_DELETE_BUTTON));
        palette->SetNoText(_(MISC_CANCEL_BUTTON));
        palette->AddText(title);
        palette->AddText(_(MISC_DELETE_ELEMENT_PROMPT_LABEL));

        if (! palette->Run())
            return;

        if (element.IsFolder()) {
            if (Git::IsGitRepo())
                Git::RemoveFolder(fullpath, name);
            else
                std::filesystem::remove_all(fullpath);
        } else {
            if (Git::IsGitRepo())
                Git::RemoveFile(fullpath, name);
            else
                std::filesystem::remove(fullpath);
        }

        passwordStoreManager->Refresh();
    }

    void RenameElementWorkflow(PasswordStoreManager *passwordStoreManager, const PasswordStoreElements::PasswordStoreElement &element) {
        auto palette = SimpleEntryPalette::Create(passwordStoreManager);
        palette->SetTitle(_(RENAME_ELEMENT_PALETTE_TITLE));
        palette->SetLabelText(_(RENAME_ELEMENT_PALETTE_LABEL_TITLE, element.GetName()));
        palette->SetYesSuggested();

        std::string newPath = "";
        if (!palette->Run([&newPath](SimpleEntryPalette *p, bool r) {
            if (r)
                newPath = p->GetEntryText();
        }))
            return;

        auto passwordStore = passwordStoreManager->GetPasswordStore();
        auto newFullPath = passwordStore.GetPath() / newPath;
        auto oldFullPath = element.GetPath();

        if (System::FileExists(newFullPath) || System::DirectoryExists(newFullPath))
            RaiseClavisError(_(ERROR_DIRECTORY_ALREADY_EXISTS, newPath));

        if (!Git::IsGitRepo() || (element.IsFolder() && System::DirectoryIsEmpty(oldFullPath)))
            std::filesystem::rename(oldFullPath, newFullPath);
        else
            Git::Move(oldFullPath, newFullPath);

        passwordStoreManager->Refresh();
    }

    void EditPasswordWorkflow(PasswordStoreManager *passwordStoreManager, const PasswordStoreElements::PasswordStoreElement &element) {
        RaiseClavisError(_(ERROR_NOT_IMPLEMENTED));
    }



}