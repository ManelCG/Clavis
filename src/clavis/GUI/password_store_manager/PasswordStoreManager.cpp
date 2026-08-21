#include <GUI/password_store_manager/PasswordStoreManager.h>

#include <algorithm>

#include <GUI/workflows/NewItemWorkflow.h>
#include <extensions/GPGWrapper.h>
#include <extensions/GUIExtensions.h>
#include <extensions/StringHelper.h>

namespace Clavis::GUI {
    PasswordStoreManager::PasswordStoreManager() :
        Gtk::Box(Gtk::Orientation::VERTICAL)
    {
        tools.set_margin_start(10);
        tools.set_margin_top(5);

        outputDisplay.set_valign(Gtk::Align::END);
        folderview.set_vexpand(true);

        searchEntry.set_placeholder_text(_(FILTER_FILES_SEARCHBAR_PLACEHOLDER));
        searchEntry.set_margin_start(10);
        searchEntry.set_margin_end(10);
        searchEntry.set_margin_top(10);

        searchEntry.signal_realize().connect([this]() {
            searchEntry.grab_focus();
        });
        searchEntry.signal_changed().connect([this]() {
            Refresh();
        });

        append(tools);
        append(searchEntry);
        append(folderview);
        append(outputDisplay);

        searchEntry.grab_focus();

        recursiveSearchCssProvider = Gtk::CssProvider::create();
        recursiveSearchCssProvider->load_from_data(
            "entry.recursive-search:focus-within { border-color: @warning_color; }"
        );
        Gtk::StyleContext::add_provider_for_display(
            Gdk::Display::get_default(),
            recursiveSearchCssProvider,
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
        );

        const auto key_controller = Gtk::EventControllerKey::create();
        key_controller->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
        key_controller->signal_key_pressed().connect(
            sigc::mem_fun(*this, &PasswordStoreManager::on_key_pressed), false);

        add_controller(key_controller);  // Attach to the entry

        refreshDispatcher.connect([this]() {
            // A pull can bring in someone else's workspace changes, so the database has to be
            // re-read before the view is rebuilt from it.
            ReloadWorkspaces();
            Refresh();
        });
    }

    PasswordStore PasswordStoreManager::GetPasswordStore() const {
        return passwordStore;
    }

    Workspaces::WorkspaceDB& PasswordStoreManager::GetWorkspaceDB() {
        return workspaceDB;
    }

    void PasswordStoreManager::ReloadWorkspaces() {
        workspaceDB = Workspaces::WorkspaceDB::Load(passwordStore.GetRoot());
    }

    bool PasswordStoreManager::IsInWorkspace() const {
        return activeWorkspace.has_value();
    }

    std::filesystem::path PasswordStoreManager::GetActiveWorkspaceDir() const {
        return activeWorkspace.has_value() ? activeWorkspace->first : std::filesystem::path{};
    }

    std::string PasswordStoreManager::GetActiveWorkspaceName() const {
        return activeWorkspace.has_value() ? activeWorkspace->second : std::string{};
    }

    void PasswordStoreManager::LeaveWorkspace() {
        activeWorkspace.reset();
    }

    void PasswordStoreManager::Initialize() {
        passwordStore = PasswordStore::Initialize();
        ReloadWorkspaces();

        folderview.SetOnElementClicked([this](const PasswordStoreElements::PasswordStoreElement& element) {
            // A workspace entry pointing at a file that is gone cannot be opened; offer to take it
            // out of the workspace instead of failing to decrypt nothing.
            if (element.IsMissing()) {
                Workflows::MissingWorkspaceElementWorkflow(this, element);
                return;
            }

            switch (element.GetType()) {
                case PasswordStoreElements::PasswordStoreElementType::WORKSPACE:
                    EnterWorkspace(element);
                    break;

                case PasswordStoreElements::PasswordStoreElementType::FOLDER:
                    Chdir(element);
                    break;

                case PasswordStoreElements::PasswordStoreElementType::GPG_FILE:
                    TryDecryptPassword(element);
                    break;

                case PasswordStoreElements::PasswordStoreElementType::TWOFA_FILE:
                    TryDecryptTwoFactor(element);
                    break;

                case PasswordStoreElements::PasswordStoreElementType::UNKNOWN:
                case PasswordStoreElements::PasswordStoreElementType::UNDEFINED:
                case PasswordStoreElements::PasswordStoreElementType::FILE_WITHOUT_EXTENSION:
                default:
                    break;
            }
        });

        folderview.SetOnDeleteItem([this](const PasswordStoreElements::PasswordStoreElement& element) {
            if (element.IsWorkspace())
                Workflows::DeleteWorkspaceWorkflow(this, element);
            else
                Workflows::DeleteElementWorkflow(this, element);
        });
        folderview.SetOnEditPassword([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::EditPasswordWorkflow(this, element);
        });
        folderview.SetOnRenameItem([this](const PasswordStoreElements::PasswordStoreElement& element) {
            if (element.IsWorkspace())
                Workflows::RenameWorkspaceWorkflow(this, element);
            else
                Workflows::RenameElementWorkflow(this, element);
        });
        folderview.SetOnExportFolder([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::ExportFolderWorkflow(this, element);
        });
        folderview.SetOnEditTwoFactor([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::EditTwoFactorWorkflow(this, element);
        });
        folderview.SetOnShowTwoFactorDetails([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::ShowTwoFactorDetailsWorkflow(this, element);
        });
        folderview.SetOnTransferTwoFactor([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::TransferTwoFactorWorkflow(this, element);
        });
        folderview.SetOnAddToWorkspace([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::AddToWorkspaceWorkflow(this, element);
        });
        folderview.SetOnEditWorkspace([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::EditWorkspaceWorkflow(this, element);
        });
        folderview.SetOnRenameInWorkspace([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::RenameInWorkspaceWorkflow(this, element);
        });
        folderview.SetOnRemoveFromWorkspace([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::RemoveFromWorkspaceWorkflow(this, element);
        });

        outputDisplay.SetOnAdvanceHotpCounter([this](const std::filesystem::path& path) {
            Workflows::AdvanceHotpCounterWorkflow(this, path);
        });

        tools.SetOnNewFolderButtonClick([this]() {
            Workflows::NewFolderWorkflow(this);
        });
        tools.SetOnNewPasswordButtonClick([this]() {
            Workflows::NewPasswordWorkflow(this);
        });
        tools.SetOnNewTwoFactorButtonClick([this]() {
            Workflows::NewTwoFactorWorkflow(this);
        });
        tools.SetOnNewWorkspaceButtonClick([this]() {
            Workflows::NewWorkspaceWorkflow(this);
        });
        tools.SetOnGoUpButtonClick([this]() {
            GoUp();
        });
        tools.SetOnRefreshButtonClick([this]() {
            Refresh();
        });
        tools.SetOnGitSync([this]() {
            refreshDispatcher.emit();
        });

        Refresh();
    }

    bool PasswordStoreManager::on_key_pressed(const guint keyval, guint keycode, Gdk::ModifierType state) {
        if (state == static_cast<Gdk::ModifierType>(0)) {
            switch (keyval) {
                case GDK_KEY_Escape:
                    if (searchEntry.get_text().empty())
                        GoUp();
                    else
                        searchEntry.set_text("");
                    return true;

                case GDK_KEY_Down:
                    folderview.ScrollDown();
                    return true;

                case GDK_KEY_Up:
                    folderview.ScrollUp();
                    return true;

                case GDK_KEY_Return:
                    folderview.ActivateFocusedItem();
                    return true;

                case GDK_KEY_F5:
                    PerformGitAction(GitManagerToolbar::Action::Sync);
                    return true;

                // Disabled keys
                case GDK_KEY_Tab:
                    return true;

                default:
                    return false;
            }
        }

        if (state == Gdk::ModifierType::CONTROL_MASK) {
            switch (keyval) {
                case GDK_KEY_c:
                    outputDisplay.TryCopyActive();
                    return true;

                case GDK_KEY_f:
                    SetRecursiveSearch(!recursiveSearchActive);
                    return true;

                case GDK_KEY_n:
                    Workflows::NewPasswordWorkflow(this);
                    searchEntry.grab_focus();
                    return true;

                case GDK_KEY_t:
                    Workflows::NewTwoFactorWorkflow(this);
                    searchEntry.grab_focus();
                    return true;

                default:
                    return false;
            }
        }

        if (state == (Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::SHIFT_MASK)) {
            switch (keyval) {
                case GDK_KEY_N:
                    Workflows::NewFolderWorkflow(this);
                    searchEntry.grab_focus();
                    return true;

                default:
                    return false;
            }
        }

        return false;
    }


    bool PasswordStoreManager::TryDecryptPassword(const PasswordStoreElements::PasswordStoreElement &elem) {
        Password p;

        if (! passwordStore.TryDecryptPassword(elem, p)) {
            outputDisplay.DisplayPasswordError();
            return false;
        }

        outputDisplay.DisplayPassword(p);
        return true;
    }

    void PasswordStoreManager::LockVault() {
        // Wipe the screen first. Flushing the agent's cache while a decrypted password or a live
        // 2FA code is still displayed would make "locked" a half-truth.
        outputDisplay.ClearAll();

        if (!GPG::TryClearPassphraseCache())
            RaiseClavisError(_(ERROR_COULD_NOT_LOCK_VAULT));
    }

    void PasswordStoreManager::DisplayTwoFactor(const TwoFactor::TwoFactorEntry &entry,
                                                const std::filesystem::path &path) {
        outputDisplay.DisplayTwoFactor(entry, path);
    }

    bool PasswordStoreManager::TryDecryptTwoFactor(const PasswordStoreElements::PasswordStoreElement &elem) {
        TwoFactor::TwoFactorEntry entry;

        if (! passwordStore.TryDecryptTwoFactor(elem, entry)) {
            outputDisplay.DisplayTwoFactorError();
            return false;
        }

        outputDisplay.DisplayTwoFactor(entry, elem.GetPath());
        return true;
    }

    void PasswordStoreManager::GoUp() {
        // Navigating away drops whatever was on screen. Note this is deliberately not done in
        // Refresh(): the search entry calls Refresh() on every keystroke, which would wipe the
        // output the moment the user started typing.
        outputDisplay.ClearAll();

        // A workspace is entered from the folder it lives in, so leaving one lands back there
        // rather than moving up a real directory.
        if (activeWorkspace.has_value())
            activeWorkspace.reset();
        else
            passwordStore.GoUp();

        searchEntry.set_text("");
        Refresh();
    }

    void PasswordStoreManager::Chdir(const PasswordStoreElements::PasswordStoreElement &elem) {
        outputDisplay.ClearAll();

        passwordStore.Chdir(elem);
        searchEntry.set_text("");
        Refresh();
    }

    void PasswordStoreManager::EnterWorkspace(const PasswordStoreElements::PasswordStoreElement &elem) {
        outputDisplay.ClearAll();

        // The element's path is virtual: storeRoot / <workspace dir> / <workspace name>. Taking it
        // apart is how the workspace is identified again, and it is also what makes entering one
        // from a recursive search result work -- the path carries the real location either way.
        const auto rel = std::filesystem::relative(elem.GetPath(), passwordStore.GetRoot());

        activeWorkspace = std::make_pair(
            Workspaces::NormalizeRelative(rel.parent_path()),
            rel.filename().string());

        searchEntry.set_text("");
        Refresh();
    }

    // A workspace is suppressed from a recursive result when one of the directories above it
    // already matched the filter, unless its own name matches independently.
    static bool IsSuppressedByMatchedAncestor(const std::filesystem::path& relPath,
                                              const std::string& ownName,
                                              const std::string& filter,
                                              bool caseSensitive) {
        auto name = caseSensitive ? ownName : StringHelper::ToLower(ownName);
        if (name.find(filter) != std::string::npos)
            return false;

        for (auto ancestor = relPath.parent_path(); !ancestor.empty(); ancestor = ancestor.parent_path()) {
            auto key = ancestor.generic_string();
            if (!caseSensitive)
                key = StringHelper::ToLower(key);

            if (key.find(filter) != std::string::npos)
                return true;
        }

        return false;
    }

    std::vector<PasswordStoreElements::PasswordStoreElement>
    PasswordStoreManager::CollectWorkspaceElements(const std::string& filter) const {
        const auto root = passwordStore.GetRoot();
        const bool caseSensitive = Settings::FILTER_CASE_SENSITIVE.GetValue();

        auto normalizedFilter = filter;
        if (!caseSensitive)
            normalizedFilter = StringHelper::ToLower(normalizedFilter);

        std::vector<PasswordStoreElements::PasswordStoreElement> ret;

        // Recursive search reaches across the whole store, so it matches on the workspace's
        // store-relative location. A plain listing only shows the ones that live right here.
        const bool isRecursive = recursiveSearchActive && !normalizedFilter.empty();
        const auto currentDir = Workspaces::ToStorageString(passwordStore.GetPath(true));

        for (const auto& workspace : workspaceDB.GetAll()) {
            const auto dir = Workspaces::ToStorageString(workspace.path);

            if (!isRecursive && dir != currentDir)
                continue;

            const auto relPath = dir.empty()
                ? std::filesystem::path(workspace.name)
                : std::filesystem::path(dir) / workspace.name;

            auto matchKey = isRecursive ? relPath.generic_string() : workspace.name;
            if (!caseSensitive)
                matchKey = StringHelper::ToLower(matchKey);

            if (!normalizedFilter.empty() && matchKey.find(normalizedFilter) == std::string::npos)
                continue;

            // Same rule the recursive listing applies to files: when a folder already matched, its
            // contents are not repeated underneath it unless they match on their own name. Without
            // this, searching for a folder would list the folder and everything inside it twice
            // over.
            if (isRecursive && IsSuppressedByMatchedAncestor(relPath, workspace.name, normalizedFilter, caseSensitive))
                continue;

            auto elem = PasswordStoreElements::PasswordStoreElement::MakeWorkspace(root / relPath);

            // Recursive results are labelled by where they live, exactly as filesystem results
            // are, so a name that appears in several folders stays distinguishable.
            if (isRecursive)
                elem.SetDisplayName(relPath.generic_string());

            ret.push_back(elem);
        }

        std::sort(ret.begin(), ret.end(), [](const auto& a, const auto& b) {
            return a.GetName() < b.GetName();
        });

        return ret;
    }

    void PasswordStoreManager::Refresh() {
        auto filter = std::string(searchEntry.get_text());

        if (activeWorkspace.has_value()) {
            Workspaces::Workspace workspace;

            // The workspace can vanish under us -- a git pull, or another window editing it. Fall
            // back to the folder it lived in rather than showing an empty view with no way out.
            if (!workspaceDB.TryGet(activeWorkspace->first, activeWorkspace->second, workspace)) {
                activeWorkspace.reset();
                Refresh();
                return;
            }

            auto elements = Workspaces::BuildElements(workspace, passwordStore.GetRoot());

            if (!filter.empty()) {
                const bool caseSensitive = Settings::FILTER_CASE_SENSITIVE.GetValue();
                auto needle = caseSensitive ? filter : StringHelper::ToLower(filter);

                std::vector<PasswordStoreElements::PasswordStoreElement> filtered;
                for (const auto& elem : elements) {
                    auto name = elem.GetName();
                    if (!caseSensitive)
                        name = StringHelper::ToLower(name);

                    if (name.find(needle) != std::string::npos)
                        filtered.push_back(elem);
                }

                elements = filtered;
            }

            folderview.DisplayElements(elements, true);

            searchEntry.grab_focus();
            tools.SetGoUpButtonActive(true);
            tools.SetCreationButtonsActive(false);
            // Not the folder it is filed under: a workspace is a place of its own, and where it
            // happens to live is not what you want to read while you are inside it.
            tools.SetPathLabel(_(WORKSPACE_PATH_LABEL, workspace.name));
            return;
        }

        // Workspaces sort ahead of every real element, so they are prepended and the existing
        // type-grouping separator logic gives them their own group for free.
        auto elements = CollectWorkspaceElements(filter);

        std::vector<PasswordStoreElements::PasswordStoreElement> storeElements;
        if (recursiveSearchActive && !filter.empty())
            storeElements = passwordStore.GetElementsRecursive(filter);
        else if (filter.empty())
            storeElements = passwordStore.GetElements();
        else
            storeElements = passwordStore.GetElements(filter);

        elements.insert(elements.end(), storeElements.begin(), storeElements.end());

        folderview.DisplayElements(elements);

        searchEntry.grab_focus();
        tools.SetGoUpButtonActive(!passwordStore.IsAtRoot());
        tools.SetCreationButtonsActive(true);
        tools.SetPath(passwordStore.GetPath(true));
    }

    void PasswordStoreManager::SetRecursiveSearch(bool active) {
        recursiveSearchActive = active;

        if (active) {
            searchEntry.set_placeholder_text(_(FILTER_FILES_SEARCHBAR_PLACEHOLDER_RECURSIVE));
            searchEntry.add_css_class("recursive-search");
        } else {
            searchEntry.set_placeholder_text(_(FILTER_FILES_SEARCHBAR_PLACEHOLDER));
            searchEntry.remove_css_class("recursive-search");
        }

        Refresh();
    }

    void PasswordStoreManager::PerformGitAction(GitManagerToolbar::Action action) {
        tools.PerformGitAction(action);
    }


}
