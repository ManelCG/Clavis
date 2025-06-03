#include <GUI/workflows/NewItemWorkflow.h>

#include <error/ClavisError.h>
#include <extensions/GPGWrapper.h>
#include <extensions/GitWrapper.h>
#include <system/Extensions.h>

#include <GUI/palettes/SimpleEntryPalette.h>
#include <GUI/palettes/NewPasswordPalette.h>
#include <GUI/palettes/SimpleYesNoQuestionPalette.h>

#include <GUI/palettes/first_run/WelcomePalette.h>
#include <GUI/palettes/first_run/ChoosePasswordStoreLocationPalette.h>
#include <GUI/palettes/first_run/GPGKeyConfigurationPalette.h>
#include <GUI/palettes/first_run/ExportGPGKeyPalette.h>
#include <GUI/palettes/first_run/ImportGPGKeyPalette.h>
#include <GUI/palettes/first_run/CreateNewGPGKeyPalette.h>

namespace Clavis::GUI {
    void Workflows::NewFolderWorkflow(PasswordStoreManager *passwordStoreManager) {
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
    void Workflows::NewPasswordWorkflow_IMPL(PasswordStoreManager *passwordStoreManager, const std::string& defaultName) {
        Password password;

        bool isEditing = !defaultName.empty();
        auto name = defaultName;
        size_t dotPos = name.rfind('.');
        if (dotPos != std::string::npos)
            name = name.substr(0, dotPos);

        if (!NewPasswordPalette::Spawn(
            passwordStoreManager,
            [&name]() {
                return new NewPasswordPalette(name);
            },
            [&password, &name](NewPasswordPalette *p, bool r) {
                if (r) {
                    password = p->GetPassword();
                    name = p->GetPasswordName();
                }
            }
        ))
            return;

        auto passwordStore = passwordStoreManager->GetPasswordStore();

        auto filename = name + ".gpg";
        auto fullpath = passwordStore.GetPath() / filename;

        if (!isEditing && passwordStore.DoesPasswordExist(filename)) {
            auto confirmPalette = SimpleYesNoQuestionPalette::Create(passwordStoreManager);

            confirmPalette->SetTitle(_(NEW_PASSWORD_PALETTE_ELEMENT_ALREADY_EXISTS_TITLE));
            confirmPalette->AddText(_(NEW_PASSWORD_PALETTE_ELEMENT_ALREADY_EXISTS_TEXT, filename));
            confirmPalette->SetYesDestructive();
            confirmPalette->SetYesText(_(MISC_OVERWRITE_BUTTON));
            confirmPalette->SetNoText(_(MISC_CANCEL_BUTTON));

            if (! confirmPalette->Run())
                return;
        }

        if (!password.TrySaveEncrypted(fullpath))
            RaiseClavisError(_(ERROR_SAVING_PASSWORD, name));

        auto relpath = std::filesystem::relative(fullpath, passwordStore.GetRoot());

        if (Git::IsGitRepo())
            Git::CommitNewFile(relpath, name);

        passwordStoreManager->Refresh();
    }

    void Workflows::NewPasswordWorkflow(PasswordStoreManager *passwordStoreManager) {
        NewPasswordWorkflow_IMPL(passwordStoreManager, "");
    }

    void Workflows::EditPasswordWorkflow(PasswordStoreManager *passwordStoreManager, const PasswordStoreElements::PasswordStoreElement &element) {
        NewPasswordWorkflow_IMPL(passwordStoreManager, element.GetName());
    }

    void Workflows::DeleteElementWorkflow(PasswordStoreManager *passwordStoreManager, const PasswordStoreElements::PasswordStoreElement &element) {
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

        auto passwordStore = passwordStoreManager->GetPasswordStore();
        auto relpath = std::filesystem::relative(fullpath, passwordStore.GetRoot());

        if (element.IsFolder()) {
            // Git wont remove empty folders
            if (!Git::IsGitRepo() || (System::DirectoryExists(fullpath) && System::ListContents(fullpath).empty()))
                std::filesystem::remove_all(fullpath);
            else
                Git::RemoveFolder(relpath, name);
        } else {
            if (Git::IsGitRepo())
                Git::RemoveFile(relpath, name);
            else
                std::filesystem::remove(fullpath);
        }

        // Ensure parent directory exists (in case Git removed it)
        auto parentPath = fullpath.parent_path();
        if (!std::filesystem::exists(parentPath)) {
            System::mkdir_p(parentPath);
        }


        passwordStoreManager->Refresh();
    }

    void Workflows::RenameElementWorkflow(PasswordStoreManager *passwordStoreManager, const PasswordStoreElements::PasswordStoreElement &element) {
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

        auto oldRelPath = std::filesystem::relative(oldFullPath, passwordStore.GetRoot());
        auto newRelPath = std::filesystem::relative(newFullPath, passwordStore.GetRoot());

        if (!Git::IsGitRepo() || (element.IsFolder() && System::DirectoryIsEmpty(oldFullPath)))
            std::filesystem::rename(oldFullPath, newFullPath);
        else
            Git::Move(oldRelPath, newRelPath);

        passwordStoreManager->Refresh();
    }

    void Workflows::ConfigGPGKeyWorkflow(PasswordStoreManager *passwordStoreManager) {
        auto palette = GPGKeyConfigurationPalette::Create(passwordStoreManager);
        std::string gpgid;
        auto response = palette->Run([&gpgid](GPGKeyConfigurationPalette* p, bool r) {
            if (r)
                gpgid = p->GetGPGID();
        });

        if (!response)
            return;

        RaiseClavisError("Selected GPGID: ", gpgid);
    }

    bool Workflows::ExportGPGWorkflow(Gtk::Window *parent) {
        auto exportKeyPalette = ExportGPGKeyPalette::Create(parent);

        auto response = exportKeyPalette->Run([](ExportGPGKeyPalette *p, bool r) {

        });

        return response;

    }

    bool Workflows::ImportGPGWorkflow(Gtk::Window *parent) {
        auto importKeyPalette = ImportGPGKeyPalette::Create(parent);

        auto response = importKeyPalette->Run([](ImportGPGKeyPalette *p, bool r) {

        });

        return response;

    }


    bool Workflows::CreateGPGWorkflow(Gtk::Window* parent) {
        auto newKeyPalette = CreateNewGPGKeyPalette::Create(parent);

        auto response = newKeyPalette->Run([](CreateNewGPGKeyPalette *p, bool r) {

        });

        return response;
    }



    void Workflows::FirstRunWorkflow(const Glib::RefPtr<Gtk::Application> &app) {

        {
            auto initializeClavisQuestion = WelcomePalette::Create();

            app->add_window(*initializeClavisQuestion);

            auto response = initializeClavisQuestion->Run([](WelcomePalette* p, bool r) {
                if (r)
                    Settings::CLAVIS_LANGUAGE.SetValue(GetLanguageCode(p->GetSelectedLanguage()));
            });

            if (!response)
                return;
        }


        std::filesystem::path passwordStoreLocation;

        {
            auto passwordStoreLocationQuestion = ChoosePasswordStoreLocationPalette::Create();

            app->add_window(*passwordStoreLocationQuestion);

            auto response = passwordStoreLocationQuestion->Run([&passwordStoreLocation](ChoosePasswordStoreLocationPalette *p, bool r) {
                if (r)
                    passwordStoreLocation = p->GetSelectedPath();
            });

            if (!response)
                return;
        }

        std::string gpgid;

        {
            auto gpgKeyPalette = GPGKeyConfigurationPalette::Create();

            app->add_window(*gpgKeyPalette);

            auto response = gpgKeyPalette->Run([&gpgid](GPGKeyConfigurationPalette *p, bool r) {
                if (r)
                    gpgid = p->GetGPGID();
            });

            if (!response)
                return;
        }

        std::cerr << passwordStoreLocation.string() << std::endl;
        std::cerr << gpgid << std::endl;
    }



}