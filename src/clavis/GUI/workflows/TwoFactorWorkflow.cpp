#include <GUI/workflows/NewItemWorkflow.h>

#include <error/ClavisError.h>
#include <extensions/GUIExtensions.h>
#include <extensions/GitWrapper.h>
#include <language/Language.h>
#include <password_store/Password.h>
#include <two_factor/MigrationImport.h>

#include <GUI/palettes/ImportTwoFactorMigrationPalette.h>
#include <GUI/palettes/NewTwoFactorPalette.h>
#include <GUI/palettes/SimpleYesNoQuestionPalette.h>
#include <GUI/palettes/TransferTwoFactorPalette.h>
#include <GUI/palettes/TwoFactorDetailsPalette.h>

namespace Clavis::GUI {
    namespace {
        // Element names carry their extension; the palettes and workflows work with the bare
        // name and re-attach ".2fa" on save.
        std::string StripTwoFactorExtension(const std::string& name) {
            const auto extension = std::string(TwoFactor::TWOFA_EXTENSION);

            if (name.size() <= extension.size())
                return name;

            if (name.compare(name.size() - extension.size(), extension.size(), extension) != 0)
                return name;

            return name.substr(0, name.size() - extension.size());
        }
    }

    bool Workflows::TryLoadTwoFactor(PasswordStoreManager* passwordStoreManager,
                                     const PasswordStoreElements::PasswordStoreElement& element,
                                     TwoFactor::TwoFactorEntry& out) {
        auto passwordStore = passwordStoreManager->GetPasswordStore();

        // Unlike editing a password, this always decrypts -- prompting if the store is locked.
        // A 2FA edit is a read-modify-write, so saving without the existing secret would destroy
        // a credential that cannot be recovered from anywhere else.
        return passwordStore.TryDecryptTwoFactor(element, out);
    }

    bool Workflows::SaveTwoFactor(PasswordStoreManager* passwordStoreManager,
                                  const std::filesystem::path& fullpath,
                                  const TwoFactor::TwoFactorEntry& entry,
                                  const std::string& name,
                                  bool isEditing) {
        auto passwordStore = passwordStoreManager->GetPasswordStore();

        auto password = Password::FromPassword(entry.Serialize());
        if (!password.TrySaveEncrypted(fullpath))
            RaiseClavisError(_(ERROR_SAVING_TWO_FACTOR, name));

        if (Git::IsGitRepo()) {
            const auto relpath = std::filesystem::relative(fullpath, passwordStore.GetRoot());
            Git::CommitFile(relpath, isEditing ? _(GIT_EDITED_TWO_FACTOR_COMMIT_MESSAGE, name)
                                               : _(GIT_ADDED_TWO_FACTOR_COMMIT_MESSAGE, name));
        }

        return true;
    }

    void Workflows::NewTwoFactorWorkflow_IMPL(PasswordStoreManager* passwordStoreManager,
                                              const std::string& defaultName,
                                              const TwoFactor::TwoFactorEntry& initial) {
        const bool isEditing = !defaultName.empty();
        auto name = StripTwoFactorExtension(defaultName);

        TwoFactor::TwoFactorEntry entry;
        std::string migrationUri;

        if (!NewTwoFactorPalette::Spawn(
            passwordStoreManager,
            [&name, &initial]() {
                return new NewTwoFactorPalette(name, initial);
            },
            [&entry, &name, &migrationUri](NewTwoFactorPalette* p, bool r) {
                if (!r)
                    return;

                if (p->WasMigrationUriPasted()) {
                    migrationUri = p->GetMigrationUri();
                    return;
                }

                entry = p->GetEntry();
                name = p->GetName();
            }
        ))
            return;

        // A bulk-export URI pasted into the single-entry dialog is handed to the importer.
        if (!migrationUri.empty()) {
            ImportTwoFactorMigrationWorkflow(passwordStoreManager, migrationUri);
            return;
        }

        if (!entry.IsValid())
            return;

        auto passwordStore = passwordStoreManager->GetPasswordStore();

        const auto filename = name + TwoFactor::TWOFA_EXTENSION;
        const auto fullpath = passwordStore.GetPath() / filename;

        if (!isEditing && passwordStore.DoesPasswordExist(filename)) {
            auto confirmPalette = SimpleYesNoQuestionPalette::Create(passwordStoreManager);

            confirmPalette->SetTitle(_(NEW_PASSWORD_PALETTE_ELEMENT_ALREADY_EXISTS_TITLE));
            confirmPalette->AddText(_(NEW_TWO_FACTOR_PALETTE_ELEMENT_ALREADY_EXISTS_TEXT, filename));
            confirmPalette->SetYesDestructive();
            confirmPalette->SetYesText(_(MISC_OVERWRITE_BUTTON));
            confirmPalette->SetNoText(_(MISC_CANCEL_BUTTON));

            if (!confirmPalette->Run())
                return;
        }

        SaveTwoFactor(passwordStoreManager, fullpath, entry, name, isEditing);

        passwordStoreManager->Refresh();
    }

    void Workflows::NewTwoFactorWorkflow(PasswordStoreManager* passwordStoreManager) {
        // See NewFolderWorkflow: nothing new is created while browsing a workspace. Editing an
        // existing entry from inside one is unaffected.
        if (passwordStoreManager->IsInWorkspace())
            return;

        NewTwoFactorWorkflow_IMPL(passwordStoreManager, "", TwoFactor::TwoFactorEntry());
    }

    void Workflows::EditTwoFactorWorkflow(PasswordStoreManager* passwordStoreManager,
                                          const PasswordStoreElements::PasswordStoreElement& element) {
        TwoFactor::TwoFactorEntry entry;
        if (!TryLoadTwoFactor(passwordStoreManager, element, entry))
            return;

        NewTwoFactorWorkflow_IMPL(passwordStoreManager, element.GetName(), entry);
    }

    void Workflows::ShowTwoFactorDetailsWorkflow(PasswordStoreManager* passwordStoreManager,
                                                 const PasswordStoreElements::PasswordStoreElement& element) {
        TwoFactor::TwoFactorEntry entry;
        if (!TryLoadTwoFactor(passwordStoreManager, element, entry))
            return;

        const auto name = StripTwoFactorExtension(element.GetName());

        Extensions::SpawnWindow<TwoFactorDetailsPalette>(
            [&name, &entry]() {
                return new TwoFactorDetailsPalette(name, entry);
            },
            passwordStoreManager);
    }

    void Workflows::TransferTwoFactorWorkflow(PasswordStoreManager* passwordStoreManager,
                                              const PasswordStoreElements::PasswordStoreElement& element) {
        TwoFactor::TwoFactorEntry entry;
        if (!TryLoadTwoFactor(passwordStoreManager, element, entry))
            return;

        const auto name = StripTwoFactorExtension(element.GetName());

        Extensions::SpawnWindow<TransferTwoFactorPalette>(
            [&name, &entry]() {
                return new TransferTwoFactorPalette(name, entry);
            },
            passwordStoreManager);
    }

    void Workflows::ImportTwoFactorMigrationWorkflow(PasswordStoreManager* passwordStoreManager,
                                                     const std::string& initialUri) {
        std::vector<std::pair<std::string, TwoFactor::TwoFactorEntry>> selected;

        if (!ImportTwoFactorMigrationPalette::Spawn(
            passwordStoreManager,
            [&initialUri]() {
                return new ImportTwoFactorMigrationPalette(initialUri);
            },
            [&selected](ImportTwoFactorMigrationPalette* p, bool r) {
                if (r)
                    selected = p->GetSelectedEntries();
            }
        ))
            return;

        if (selected.empty())
            return;

        auto passwordStore = passwordStoreManager->GetPasswordStore();

        std::vector<std::filesystem::path> importedPaths;
        for (const auto& [name, entry] : selected) {
            const auto fullpath = passwordStore.GetPath() / (name + TwoFactor::TWOFA_EXTENSION);

            auto password = Password::FromPassword(entry.Serialize());
            if (!password.TrySaveEncrypted(fullpath))
                RaiseClavisError(_(ERROR_SAVING_TWO_FACTOR, name));

            importedPaths.push_back(std::filesystem::relative(fullpath, passwordStore.GetRoot()));
        }

        // One commit for the whole batch rather than one per account.
        if (Git::IsGitRepo() && !importedPaths.empty())
            Git::CommitImport(importedPaths, std::to_string(importedPaths.size()));

        passwordStoreManager->Refresh();
    }

    void Workflows::AdvanceHotpCounterWorkflow(PasswordStoreManager* passwordStoreManager,
                                               const std::filesystem::path& fullpath) {
        auto passwordStore = passwordStoreManager->GetPasswordStore();

        // Re-read from disk rather than trusting the in-memory copy, so a counter that moved on
        // via `git pull` is respected. The file is the single source of truth.
        const auto element = PasswordStoreElements::PasswordStoreElement(fullpath);

        TwoFactor::TwoFactorEntry entry;
        if (!passwordStore.TryDecryptTwoFactor(element, entry))
            return;

        if (entry.GetType() != TwoFactor::OtpType::HOTP)
            return;

        entry.AdvanceCounter();

        const auto name = StripTwoFactorExtension(element.GetName());

        auto password = Password::FromPassword(entry.Serialize());
        if (!password.TrySaveEncrypted(fullpath))
            RaiseClavisError(_(ERROR_SAVING_TWO_FACTOR, name));

        if (Git::IsGitRepo()) {
            const auto relpath = std::filesystem::relative(fullpath, passwordStore.GetRoot());
            Git::CommitFile(relpath, _(GIT_ADVANCED_HOTP_COUNTER_COMMIT_MESSAGE, name));
        }

        passwordStoreManager->DisplayTwoFactor(entry, fullpath);
    }
}
