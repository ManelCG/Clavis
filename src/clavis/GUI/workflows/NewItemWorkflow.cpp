#include <GUI/workflows/NewItemWorkflow.h>

#include <error/ClavisError.h>
#include <extensions/GPGWrapper.h>
#include <extensions/GitWrapper.h>
#include <system/Extensions.h>

#include <GUI/lib/MainLoopHalter.h>

#include <GUI/palettes/SimpleEntryPalette.h>
#include <GUI/palettes/NewPasswordPalette.h>
#include <GUI/palettes/SimpleYesNoQuestionPalette.h>
#include <GUI/palettes/ExportPasswordStorePalette.h>
#include <GUI/palettes/ImportPasswordStorePalette.h>
#include <clav/ClavFile.h>

#include <GUI/palettes/ExceptionPalette.h>
#include <GUI/palettes/first_run/WelcomePalette.h>
#include <GUI/palettes/first_run/ChoosePasswordStoreLocationPalette.h>
#include <GUI/palettes/first_run/GPGKeyConfigurationPalette.h>
#include <GUI/palettes/first_run/ExportGPGKeyPalette.h>
#include <GUI/palettes/first_run/CreateNewGPGKeyPalette.h>
#include <GUI/palettes/first_run/GitServerConfigPalette.h>

#ifdef __WINDOWS__
#include <windows.h>
#include <shobjidl.h>    // for IFileDialog
#include <shlobj.h>      // for SHBrowseForFolder
#include <commdlg.h>     // for GetOpenFileName, GetSaveFileName
#include <string>
#endif

namespace Clavis::GUI {
    void Workflows::NewFolderWorkflow(PasswordStoreManager *passwordStoreManager) {
        auto palette = SimpleEntryPalette::Create(passwordStoreManager);
        palette->SetTitle(_(NEW_FOLDER_PALETTE_TITLE));
        palette->SetLabelText(_(NEW_FOLDER_PALETTE_LABEL_TITLE));
        palette->SetYesSuggested();
        palette->SetIsEntryRequiredForYes(true);

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
            // Git won't remove empty folders; also fall back if git rm fails (untracked)
            if (!Git::IsGitRepo() || (System::DirectoryExists(fullpath) && System::ListContents(fullpath).empty()))
                std::filesystem::remove_all(fullpath);
            else if (!Git::RemoveFolder(relpath, name))
                std::filesystem::remove_all(fullpath);
        } else {
            if (!Git::IsGitRepo() || !Git::RemoveFile(relpath, name))
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

    bool Workflows::OpenFileDialog(FileOpenDialogAction action, std::string &outSelectedPath, Gtk::Widget *parent) {
	#ifdef __UNIX__
        auto window = Extensions::GetParentWindow(parent);
        auto dialog = Gtk::FileDialog::create();

        if (window != nullptr) {
            dialog->set_modal(true);
            window->set_sensitive(false);
        }

        if (!outSelectedPath.empty()) {
            if (action == FileOpenDialogAction::OPEN_FOLDER && System::DirectoryExists(outSelectedPath))
                dialog->set_initial_folder(Gio::File::create_for_path(outSelectedPath));
            else
                dialog->set_initial_name(outSelectedPath);
        }

        MainLoopHalter halter;
        bool response;

        auto slot = [dialog, &halter, &outSelectedPath, &response, &action](const Glib::RefPtr<Gio::AsyncResult>& result) {
            try {
                Glib::RefPtr<Gio::File> file;
                switch (action) {
                    case FileOpenDialogAction::OPEN_FILE:
                        file = dialog->open_finish(result);
                        break;
                    case FileOpenDialogAction::SAVE_FILE:
                        file = dialog->save_finish(result);
                        break;
                    case FileOpenDialogAction::OPEN_FOLDER:
                        file = dialog->select_folder_finish(result);
                        break;
                }

                if (file) {
                    outSelectedPath = file->get_path();
                    response = true;
                }
                else
                    response = false;

            } catch (...) {
                response = false;
            }

            halter.Resume();
        };

        switch (action) {
            case FileOpenDialogAction::OPEN_FILE:
                if (window != nullptr)
                    dialog->open(*window, slot);
                else
                    dialog->open(slot);
                break;
            case FileOpenDialogAction::SAVE_FILE:
                if (window != nullptr)
                    dialog->save(*window, slot);
                else
                    dialog->save(slot);
                break;
            case FileOpenDialogAction::OPEN_FOLDER:
                if (window != nullptr)
                    dialog->select_folder(*window, slot);
                else
                    dialog->select_folder(slot);
                break;
        }

        halter.Halt();
        if (window != nullptr)
            window->set_sensitive(true);
        return response;
    #elif defined __WINDOWS__
        wchar_t filePath[MAX_PATH] = { 0 };

        switch (action) {
        case FileOpenDialogAction::OPEN_FILE: {
            OPENFILENAMEW ofn = { 0 };
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = nullptr;
            ofn.lpstrFile = filePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameW(&ofn)) {
                outSelectedPath = System::UnicodeToUTF8(filePath);
                return true;
            }
            return false;
        }

        case FileOpenDialogAction::SAVE_FILE: {
            OPENFILENAMEW ofn = { 0 };
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = nullptr;
            ofn.lpstrFile = filePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

            if (GetSaveFileNameW(&ofn)) {
                outSelectedPath = System::UnicodeToUTF8(filePath);
                return true;
            }
            return false;
        }

        case FileOpenDialogAction::OPEN_FOLDER: {
            IFileDialog* pfd = nullptr;
            HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (SUCCEEDED(hr)) {
                hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                    IID_PPV_ARGS(&pfd));

                if (SUCCEEDED(hr)) {
                    DWORD options;
                    pfd->GetOptions(&options);
                    pfd->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

                    if (SUCCEEDED(pfd->Show(nullptr))) {
                        IShellItem* pItem = nullptr;
                        if (SUCCEEDED(pfd->GetResult(&pItem))) {
                            PWSTR pszFilePath = nullptr;
                            if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {
                                outSelectedPath = System::UnicodeToUTF8(pszFilePath);
                                CoTaskMemFree(pszFilePath);
                                pItem->Release();
                                pfd->Release();
                                CoUninitialize();
                                return true;
                            }
                            pItem->Release();
                        }
                    }
                    pfd->Release();
                }
                CoUninitialize();
            }
            return false;
        }
        }

        return false;
    #endif
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

    bool Workflows::ExportGPGWorkflow(const std::string& gpgid, Gtk::Window *parent) {
        auto exportKeyPalette = ExportGPGKeyPalette::Create(parent);

        auto response = exportKeyPalette->Run([gpgid](ExportGPGKeyPalette *p, bool r) {
            if (r) {
                auto path = p->GetExportPath();
                auto doExportPrivate = p->GetDoExportPrivate();

                std::vector<uint8_t> data;
                if (!GPG::TryExportKey(gpgid, doExportPrivate, data))
                    RaiseClavisError(_(ERROR_UNABLE_TO_EXPORT_KEY))

                if (!System::TryWriteFile(path, data))
                    RaiseClavisError(_(ERROR_COULD_NOT_WRITE_FILE, path.string()));
            }
        });

        return response;

    }

    bool Workflows::ImportGPGWorkflow(Gtk::Window *parent, std::string& outFingerprint) {
        std::string path;
        if (!OpenFileDialog(FileOpenDialogAction::OPEN_FILE, path, parent))
            return false;

        std::vector<uint8_t> data;
        if (!System::TryReadFile(path, data))
            RaiseClavisError(_(ERROR_COULD_NOT_READ_FILE, path));

        if (!GPG::TryImportKey(data, outFingerprint))
            RaiseClavisError(_(ERROR_UNABLE_TO_IMPORT_KEY))

        return true;
    }


    bool Workflows::CreateGPGWorkflow(Gtk::Window* parent, std::string& outFingerprint) {
        auto newKeyPalette = CreateNewGPGKeyPalette::Create(parent);

        auto response = newKeyPalette->Run([&outFingerprint](CreateNewGPGKeyPalette *p, bool r) {
            if (r) {
                auto key = p->GetKey();
                if (!GPG::TryCreateKey(key, outFingerprint))
                    RaiseClavisError(_(ERROR_FAILED_CREATING_KEY, GPG::KeyToString(key, true)));
            }
        });

        return response;
    }



    void Workflows::ExportPasswordStoreWorkflow(PasswordStoreManager* passwordStoreManager, Gtk::Window* parent) {
        const auto store      = passwordStoreManager->GetPasswordStore();
        const auto exportName = store.GetRoot().filename().string();

        auto palette = ExportPasswordStorePalette::Create(parent, [&exportName]() {
            return new ExportPasswordStorePalette("", "export.clav");
        });

        std::filesystem::path   exportPath;
        std::string             password;
        Clav::EncryptionType    encMode = Clav::EncryptionType::None;

        const bool confirmed = palette->Run([&](ExportPasswordStorePalette* p, bool r) {
            if (!r) return;
            exportPath = p->GetExportPath();
            password   = p->GetPassword();
            encMode    = p->GetEncryptionMode();
        });

        if (!confirmed)
            return;

        auto fileOut = Clav::ClavFile::Build(store, exportName, encMode, password);

        if (!System::TryWriteFile(exportPath, fileOut))
            RaiseClavisError(_(ERROR_COULD_NOT_WRITE_FILE, exportPath.string()));
    }

    void Workflows::ExportFolderWorkflow(PasswordStoreManager* passwordStoreManager, const PasswordStoreElements::PasswordStoreElement& folder) {
        const auto store          = passwordStoreManager->GetPasswordStore();
        const auto folderName     = folder.GetName();
        const auto defaultFilename = folder.GetPath().filename().string() + ".clav";

        auto palette = ExportPasswordStorePalette::Create(passwordStoreManager, [&folderName, &defaultFilename]() {
            return new ExportPasswordStorePalette(folderName, defaultFilename);
        });

        std::filesystem::path   exportPath;
        std::string             password;
        Clav::EncryptionType    encMode = Clav::EncryptionType::None;

        const bool confirmed = palette->Run([&](ExportPasswordStorePalette* p, bool r) {
            if (!r) return;
            exportPath = p->GetExportPath();
            password   = p->GetPassword();
            encMode    = p->GetEncryptionMode();
        });

        if (!confirmed)
            return;

        auto fileOut = Clav::ClavFile::Build(store, folder.GetPath(), folderName, encMode, password);

        if (!System::TryWriteFile(exportPath, fileOut))
            RaiseClavisError(_(ERROR_COULD_NOT_WRITE_FILE, exportPath.string()));
    }

    void Workflows::ImportPasswordStoreWorkflow(PasswordStoreManager* passwordStoreManager, Gtk::Window* parent) {
        auto palette = ImportPasswordStorePalette::Create(parent, []() {
            return new ImportPasswordStorePalette();
        });

        std::filesystem::path filePath;
        std::string           password;

        const bool confirmed = palette->Run([&](ImportPasswordStorePalette* p, bool r) {
            if (!r) return;
            filePath = p->GetFilePath();
            password = p->GetPassword();
        });

        if (!confirmed)
            return;

        std::vector<uint8_t> fileData;
        if (!System::TryReadFile(filePath, fileData))
            RaiseClavisError(_(ERROR_IMPORT_FAILED));

        Clav::ParsedClavFile parsed;
        const auto result = Clav::ClavFile::TryRead(fileData, parsed, password);

        if (result != Clav::ClavReadResult::Ok)
            RaiseClavisError(_(ERROR_IMPORT_FAILED));

        if (!parsed.publicKeyData.empty() && !Clav::ClavFile::CheckPublicKeyMatchesStore(parsed.publicKeyData))
            RaiseClavisError(_(ERROR_IMPORT_GPG_KEY_MISMATCH));

        std::string destPath = passwordStoreManager->GetPasswordStore().GetRoot().string();
        if (!OpenFileDialog(FileOpenDialogAction::OPEN_FOLDER, destPath, parent))
            return;

        if (!Clav::ClavFile::Unpack(parsed, destPath))
            RaiseClavisError(_(ERROR_IMPORT_FAILED));

        if (Git::IsGitRepo()) {
            std::vector<std::filesystem::path> importedPaths;
            for (const auto& entry : parsed.entries)
                importedPaths.push_back(std::filesystem::path(destPath) / entry.relPath);
            Git::CommitImport(importedPaths, parsed.name);
        }

        passwordStoreManager->Refresh();
    }

    bool Workflows::FirstRunWorkflow(const Glib::RefPtr<Gtk::Application> &app) {
        Glib::add_exception_handler([]() {
            auto data = Error::ClavisException::GetLastException();

            const auto p = Gtk::make_managed<ExceptionPalette>(data);
            p->show();
        });

        /* Initialize Clavis? */ {
            auto initializeClavisQuestion = WelcomePalette::Create();

            app->add_window(*initializeClavisQuestion);

            auto response = initializeClavisQuestion->Run([](WelcomePalette* p, bool r) {
                if (r)
                    Settings::CLAVIS_LANGUAGE.SetValue(GetLanguageCode(p->GetSelectedLanguage()));
            });

            if (!response)
                return false;
        }


        std::filesystem::path passwordStoreLocation;

        /* Choose Password Store location */ {
            auto passwordStoreLocationQuestion = ChoosePasswordStoreLocationPalette::Create();

            app->add_window(*passwordStoreLocationQuestion);

            auto response = passwordStoreLocationQuestion->Run([&passwordStoreLocation](ChoosePasswordStoreLocationPalette *p, bool r) {
                if (r)
                    passwordStoreLocation = p->GetSelectedPath();
            });

            if (!response)
                return false;
        }
        bool passwordStoreAlreadyExists = System::DirectoryExists(passwordStoreLocation) && PasswordStore::IsValidPasswordStore(passwordStoreLocation);

        // Git config palette does everything by itself because it is supposed to be responsive.
        // Also, the user might not want to use Git, so we don't really need to do anything here
        if (!System::DirectoryExists(passwordStoreLocation) || !passwordStoreAlreadyExists) {
            // Try to initialize git repo
            auto gitConfigPalette = GitServerConfigPalette::Create(nullptr, [&passwordStoreLocation]() {
                return new GitServerConfigPalette(passwordStoreLocation);
            });

            app->add_window(*gitConfigPalette);

            if (!gitConfigPalette->Run())
                return false;
        }

        // Update this since the git manager can clone the password store
        passwordStoreAlreadyExists = System::DirectoryExists(passwordStoreLocation) && PasswordStore::IsValidPasswordStore(passwordStoreLocation);

        std::string gpgid;

        const bool gpgKeyExists =
            passwordStoreAlreadyExists &&
            PasswordStore::TryGetGPGID(passwordStoreLocation, gpgid) &&
            GPG::KeyExists(gpgid);

        // Choose GPG key
        if (!gpgKeyExists) {
            auto gpgKeyPalette = GPGKeyConfigurationPalette::Create();

            app->add_window(*gpgKeyPalette);

            const auto response = gpgKeyPalette->Run([&gpgid](GPGKeyConfigurationPalette *p, bool r) {
                if (r)
                    gpgid = p->GetGPGID();
            });

            if (!response)
                return false;
        }

        // We are done!
        // Create the password store path if necessary
        if (! System::DirectoryExists(passwordStoreLocation))
            if (!System::mkdir_p(passwordStoreLocation))
                RaiseClavisError(_(ERROR_UNABLE_TO_CREATE_DIRECTORY, passwordStoreLocation));

        // Set the password store path in settings to reuse now.
        Settings::PASSWORD_STORE_PATH.SetValue(passwordStoreLocation.string());

        auto gpgidPath = System::GetGPGIDPath();
        if (!System::FileExists(gpgidPath))
            if (!System::TryWriteFile(gpgidPath, gpgid))
                RaiseClavisError(_(ERROR_COULD_NOT_WRITE_FILE, gpgidPath));


        // Done!
        Settings::IS_FIRST_RUN.SetValue(false);
        return true;
    }



}