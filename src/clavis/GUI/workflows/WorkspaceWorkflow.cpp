#include <GUI/workflows/NewItemWorkflow.h>

#include <error/ClavisError.h>
#include <language/Language.h>
#include <system/Extensions.h>

#include <GUI/palettes/SimpleEntryPalette.h>
#include <GUI/palettes/SimpleYesNoQuestionPalette.h>
#include <GUI/palettes/WorkspacePalette.h>
#include <GUI/palettes/WorkspacePickerPalette.h>

namespace Clavis::GUI {
    void Workflows::SaveWorkspaces(PasswordStoreManager* passwordStoreManager, const std::string& commitMessage) {
        if (!passwordStoreManager->GetWorkspaceDB().Save(commitMessage))
            RaiseClavisError(_(ERROR_UNABLE_TO_SAVE_WORKSPACES));
    }

    bool Workflows::TryResolveWorkspace(PasswordStoreManager* passwordStoreManager,
                                        const PasswordStoreElements::PasswordStoreElement& element,
                                        std::filesystem::path& outDir, std::string& outName) {
        if (!element.IsWorkspace())
            return false;

        // The element's path is virtual -- storeRoot / <dir> / <name> -- and taking it apart is
        // what identifies the workspace again, whether it was listed in its own folder or found
        // through a recursive search.
        const auto root = passwordStoreManager->GetPasswordStore().GetRoot();
        const auto rel = std::filesystem::relative(element.GetPath(), root);

        outDir = Workspaces::NormalizeRelative(rel.parent_path());
        outName = rel.filename().string();

        return passwordStoreManager->GetWorkspaceDB().Exists(outDir, outName);
    }

    void Workflows::NewWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager) {
        auto& db = passwordStoreManager->GetWorkspaceDB();
        const auto dir = Workspaces::NormalizeRelative(passwordStoreManager->GetPasswordStore().GetPath(true));

        Workspaces::Workspace workspace;
        workspace.path = dir;

        Workspaces::Workspace result;
        if (!WorkspacePalette::Spawn(
            passwordStoreManager,
            [&workspace]() { return new WorkspacePalette(workspace, false); },
            [&result](WorkspacePalette* p, bool r) {
                if (!r)
                    return;

                result.name = p->GetWorkspaceName();
                result.entries = p->GetEntries();
            }
        ))
            return;

        result.path = dir;

        if (db.Exists(dir, result.name))
            RaiseClavisError(_(ERROR_WORKSPACE_ALREADY_EXISTS, result.name));

        db.Upsert(result);
        SaveWorkspaces(passwordStoreManager, _(GIT_WORKSPACE_CREATED_COMMIT_MESSAGE, result.name));

        passwordStoreManager->Refresh();
    }

    void Workflows::EditWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager,
                                          const PasswordStoreElements::PasswordStoreElement& element) {
        auto& db = passwordStoreManager->GetWorkspaceDB();

        std::filesystem::path dir;
        std::string name;
        if (!TryResolveWorkspace(passwordStoreManager, element, dir, name))
            RaiseClavisError(_(ERROR_WORKSPACE_NOT_FOUND, element.GetLabel()));

        Workspaces::Workspace workspace;
        db.TryGet(dir, name, workspace);

        Workspaces::Workspace result;
        if (!WorkspacePalette::Spawn(
            passwordStoreManager,
            [&workspace]() { return new WorkspacePalette(workspace, true); },
            [&result](WorkspacePalette* p, bool r) {
                if (!r)
                    return;

                result.name = p->GetWorkspaceName();
                result.entries = p->GetEntries();
            }
        ))
            return;

        result.path = dir;

        // Renaming through the dialog is a rename like any other, so it has to respect the same
        // uniqueness rule the rename dialog does.
        const bool isRenamed = result.name != name;
        if (isRenamed && db.Exists(dir, result.name))
            RaiseClavisError(_(ERROR_WORKSPACE_ALREADY_EXISTS, result.name));

        if (isRenamed)
            db.Remove(dir, name);

        db.Upsert(result);
        SaveWorkspaces(passwordStoreManager, isRenamed
            ? _(GIT_WORKSPACE_RENAMED_COMMIT_MESSAGE, name, result.name)
            : _(GIT_WORKSPACE_UPDATED_COMMIT_MESSAGE, result.name));

        // The view may be sitting inside the workspace that was just renamed out from under it.
        // Any other workspace it might be showing is unaffected and should stay open.
        if (isRenamed &&
            passwordStoreManager->GetActiveWorkspaceDir() == dir &&
            passwordStoreManager->GetActiveWorkspaceName() == name)
            passwordStoreManager->LeaveWorkspace();

        passwordStoreManager->Refresh();
    }

    void Workflows::RenameWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager,
                                            const PasswordStoreElements::PasswordStoreElement& element) {
        auto& db = passwordStoreManager->GetWorkspaceDB();

        std::filesystem::path dir;
        std::string name;
        if (!TryResolveWorkspace(passwordStoreManager, element, dir, name))
            RaiseClavisError(_(ERROR_WORKSPACE_NOT_FOUND, element.GetLabel()));

        auto palette = SimpleEntryPalette::Create(passwordStoreManager);
        palette->SetTitle(_(WORKSPACE_RENAME_TITLE));
        palette->SetLabelText(_(WORKSPACE_RENAME_LABEL, name));
        palette->SetEntryText(name);
        palette->SetIsEntryRequiredForYes(true);
        palette->SetYesSuggested();

        std::string input;
        if (!palette->Run([&input](SimpleEntryPalette* p, bool r) {
            if (r)
                input = p->GetEntryText();
        }))
            return;

        // The input doubles as a move: "Other/Work" puts the workspace in Other, and "../Work"
        // pushes it up a level. Resolved against the workspace's own directory, exactly as
        // renaming a file is resolved against the directory that file sits in.
        const auto combined = (dir / input).lexically_normal();
        const auto newDir = Workspaces::NormalizeRelative(combined.parent_path());
        const auto newName = combined.filename().string();

        if (newName.empty())
            RaiseClavisError(_(ERROR_WORKSPACE_INVALID_PATH, input));

        if (newDir == dir && newName == name)
            return;

        // A workspace has to live somewhere real: it is listed alongside that directory's
        // contents, and a directory that does not exist is never browsed.
        const auto root = passwordStoreManager->GetPasswordStore().GetRoot();
        const auto targetDir = (root / newDir).lexically_normal();
        const auto rootNormalized = root.lexically_normal();

        if (!System::DirectoryExists(targetDir))
            RaiseClavisError(_(ERROR_WORKSPACE_INVALID_PATH, input));

        // "../.." out of the store would put the workspace somewhere Clavis never lists.
        const auto relToRoot = std::filesystem::relative(targetDir, rootNormalized);
        if (relToRoot.empty() || *relToRoot.begin() == "..")
            RaiseClavisError(_(ERROR_WORKSPACE_INVALID_PATH, input));

        if (db.Exists(newDir, newName))
            RaiseClavisError(_(ERROR_WORKSPACE_ALREADY_EXISTS, newName));

        db.Move(dir, name, newDir, newName);

        // Moving and renaming go through the same dialog, but they are different events in the
        // history and reading "moved" where nothing moved would be misleading.
        const auto newRelPath = newDir.empty()
            ? std::filesystem::path(newName)
            : newDir / newName;

        SaveWorkspaces(passwordStoreManager, newDir == dir
            ? _(GIT_WORKSPACE_RENAMED_COMMIT_MESSAGE, name, newName)
            : _(GIT_WORKSPACE_MOVED_COMMIT_MESSAGE, name, newRelPath.generic_string()));

        if (passwordStoreManager->GetActiveWorkspaceDir() == dir &&
            passwordStoreManager->GetActiveWorkspaceName() == name)
            passwordStoreManager->LeaveWorkspace();

        passwordStoreManager->Refresh();
    }

    void Workflows::DeleteWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager,
                                            const PasswordStoreElements::PasswordStoreElement& element) {
        auto& db = passwordStoreManager->GetWorkspaceDB();

        std::filesystem::path dir;
        std::string name;
        if (!TryResolveWorkspace(passwordStoreManager, element, dir, name))
            RaiseClavisError(_(ERROR_WORKSPACE_NOT_FOUND, element.GetLabel()));

        auto palette = SimpleYesNoQuestionPalette::Create(passwordStoreManager);

        const auto title = _(WORKSPACE_DELETE_PROMPT, name);
        palette->SetTitle(title);
        palette->SetYesDestructive();
        palette->SetYesText(_(MISC_DELETE_BUTTON));
        palette->SetNoText(_(MISC_CANCEL_BUTTON));
        palette->AddText(title);
        palette->AddText(_(WORKSPACE_DELETE_PROMPT_LABEL));

        if (!palette->Run())
            return;

        db.Remove(dir, name);
        SaveWorkspaces(passwordStoreManager, _(GIT_WORKSPACE_DELETED_COMMIT_MESSAGE, name));

        if (passwordStoreManager->GetActiveWorkspaceDir() == dir &&
            passwordStoreManager->GetActiveWorkspaceName() == name)
            passwordStoreManager->LeaveWorkspace();

        passwordStoreManager->Refresh();
    }

    void Workflows::AddToWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager,
                                           const PasswordStoreElements::PasswordStoreElement& element) {
        auto& db = passwordStoreManager->GetWorkspaceDB();

        const auto root = passwordStoreManager->GetPasswordStore().GetRoot();
        const auto target = Workspaces::NormalizeRelative(
            std::filesystem::relative(element.GetPath(), root));

        // Not element.GetLabel(): a result found through recursive search carries its whole
        // store-relative path as its display name, which would make a poor default alias.
        const auto label = PasswordStoreElements::PasswordStoreElement(element.GetPath()).GetLabel();
        const auto workspaces = db.GetAll();

        std::optional<Workspaces::Workspace> chosen;
        if (!WorkspacePickerPalette::Spawn(
            passwordStoreManager,
            [&workspaces, &label]() { return new WorkspacePickerPalette(workspaces, label); },
            [&chosen](WorkspacePickerPalette* p, bool r) {
                if (r)
                    chosen = p->GetChosen();
            }
        ))
            return;

        if (!chosen.has_value())
            return;

        // The prompt starts empty with the file's own name as a placeholder, so confirming without
        // typing keeps that name -- the common case -- while still inviting a better one.
        auto namePalette = SimpleEntryPalette::Create(passwordStoreManager);
        namePalette->SetTitle(_(WORKSPACE_ADD_NAME_PROMPT_TITLE));
        namePalette->SetLabelText(_(WORKSPACE_ADD_NAME_PROMPT_LABEL, label));
        namePalette->SetEntryPlaceholder(label);
        namePalette->SetYesSuggested();

        std::string entryName;
        if (!namePalette->Run([&entryName](SimpleEntryPalette* p, bool r) {
            if (r)
                entryName = p->GetEntryText();
        }))
            return;

        if (entryName.empty())
            entryName = label;

        Workspaces::WorkspaceEntry entry;
        entry.name = entryName;
        entry.target = target;

        if (!db.AddEntry(chosen->path, chosen->name, entry))
            RaiseClavisError(_(ERROR_WORKSPACE_TARGET_ALREADY_PRESENT, label, chosen->name));

        SaveWorkspaces(passwordStoreManager,
            _(GIT_WORKSPACE_ENTRY_ADDED_COMMIT_MESSAGE, target.generic_string(), entryName, chosen->name));
        passwordStoreManager->Refresh();
    }

    void Workflows::RemoveFromWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager,
                                                const PasswordStoreElements::PasswordStoreElement& element) {
        if (!passwordStoreManager->IsInWorkspace())
            return;

        auto& db = passwordStoreManager->GetWorkspaceDB();

        const auto root = passwordStoreManager->GetPasswordStore().GetRoot();
        const auto target = Workspaces::NormalizeRelative(
            std::filesystem::relative(element.GetPath(), root));

        // No confirmation: nothing is destroyed, and putting the entry back is two clicks away.
        if (!db.RemoveEntry(passwordStoreManager->GetActiveWorkspaceDir(),
                            passwordStoreManager->GetActiveWorkspaceName(), target))
            return;

        SaveWorkspaces(passwordStoreManager, _(GIT_WORKSPACE_ENTRY_REMOVED_COMMIT_MESSAGE,
            target.generic_string(), passwordStoreManager->GetActiveWorkspaceName()));
        passwordStoreManager->Refresh();
    }

    void Workflows::RenameInWorkspaceWorkflow(PasswordStoreManager* passwordStoreManager,
                                              const PasswordStoreElements::PasswordStoreElement& element) {
        if (!passwordStoreManager->IsInWorkspace())
            return;

        auto& db = passwordStoreManager->GetWorkspaceDB();

        const auto root = passwordStoreManager->GetPasswordStore().GetRoot();
        const auto target = Workspaces::NormalizeRelative(
            std::filesystem::relative(element.GetPath(), root));

        // The stored alias is the truth here. The element's display name falls back to the
        // entry's path when two aliases collide, which must not become the prefilled name.
        Workspaces::Workspace workspace;
        if (!db.TryGet(passwordStoreManager->GetActiveWorkspaceDir(),
                       passwordStoreManager->GetActiveWorkspaceName(), workspace))
            return;

        std::string currentName;
        for (const auto& candidate : workspace.entries)
            if (Workspaces::ToStorageString(candidate.target) == Workspaces::ToStorageString(target))
                currentName = candidate.name;

        auto palette = SimpleEntryPalette::Create(passwordStoreManager);
        palette->SetTitle(_(WORKSPACE_RENAME_ENTRY_TITLE));
        palette->SetLabelText(_(WORKSPACE_RENAME_ENTRY_LABEL, target.filename().string()));
        palette->SetEntryText(currentName);
        palette->SetIsEntryRequiredForYes(true);
        palette->SetYesSuggested();

        std::string newName;
        if (!palette->Run([&newName](SimpleEntryPalette* p, bool r) {
            if (r)
                newName = p->GetEntryText();
        }))
            return;

        if (!db.RenameEntry(passwordStoreManager->GetActiveWorkspaceDir(),
                            passwordStoreManager->GetActiveWorkspaceName(), target, newName))
            return;

        SaveWorkspaces(passwordStoreManager, _(GIT_WORKSPACE_ENTRY_RENAMED_COMMIT_MESSAGE,
            currentName, newName, passwordStoreManager->GetActiveWorkspaceName()));
        passwordStoreManager->Refresh();
    }

    void Workflows::MissingWorkspaceElementWorkflow(PasswordStoreManager* passwordStoreManager,
                                                    const PasswordStoreElements::PasswordStoreElement& element) {
        if (!passwordStoreManager->IsInWorkspace())
            return;

        auto palette = SimpleYesNoQuestionPalette::Create(passwordStoreManager);

        palette->SetTitle(_(WORKSPACE_ELEMENT_MISSING_TOOLTIP));
        palette->SetYesDestructive();
        palette->SetYesText(_(MISC_DELETE_BUTTON));
        palette->SetNoText(_(MISC_CANCEL_BUTTON));
        palette->AddText(_(WORKSPACE_ELEMENT_MISSING_PROMPT, element.GetLabel()));

        if (!palette->Run())
            return;

        RemoveFromWorkspaceWorkflow(passwordStoreManager, element);
    }
}
