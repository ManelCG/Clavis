#include <GUI/password_store_manager/PasswordStoreManager.h>

#include <GUI/workflows/NewItemWorkflow.h>

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

        const auto key_controller = Gtk::EventControllerKey::create();
        key_controller->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
        key_controller->signal_key_pressed().connect(
            sigc::mem_fun(*this, &PasswordStoreManager::on_key_pressed), false);

        add_controller(key_controller);  // Attach to the entry

        refreshDispatcher.connect([this]() {
            Refresh();
        });
    }

    PasswordStore PasswordStoreManager::GetPasswordStore() const {
        return passwordStore;
    }

    void PasswordStoreManager::Initialize() {
        passwordStore = PasswordStore::Initialize();

        folderview.SetOnElementClicked([this](const PasswordStoreElements::PasswordStoreElement& element) {
            switch (element.GetType()) {
                case PasswordStoreElements::PasswordStoreElementType::FOLDER:
                    Chdir(element);
                    break;

                case PasswordStoreElements::PasswordStoreElementType::GPG_FILE:
                    TryDecryptPassword(element);
                    break;

                case PasswordStoreElements::PasswordStoreElementType::UNKNOWN:
                case PasswordStoreElements::PasswordStoreElementType::UNDEFINED:
                case PasswordStoreElements::PasswordStoreElementType::FILE_WITHOUT_EXTENSION:
                default:
                    break;
            }
        });

        folderview.SetOnDeleteItem([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::DeleteElementWorkflow(this, element);
        });
        folderview.SetOnEditPassword([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::EditPasswordWorkflow(this, element);
        });
        folderview.SetOnRenameItem([this](const PasswordStoreElements::PasswordStoreElement& element) {
            Workflows::RenameElementWorkflow(this, element);
        });

        tools.SetOnNewFolderButtonClick([this]() {
            Workflows::NewFolderWorkflow(this);
        });
        tools.SetOnNewPasswordButtonClick([this]() {
            Workflows::NewPasswordWorkflow(this);
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
        if (state == Gdk::ModifierType::NO_MODIFIER_MASK) {
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

                // Disabled keys
                case GDK_KEY_Tab:
                    return true;

                default:
                    return false;
            }
        }

        if (state == Gdk::ModifierType::CONTROL_MASK) {
            switch (keyval) {
                case GDK_KEY_f:
                    RaiseClavisError("Ctrl F!");
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
            outputDisplay.DisplayError();
            return false;
        }

        outputDisplay.DisplayPassword(p);
        return true;
    }

    void PasswordStoreManager::GoUp() {
        passwordStore.GoUp();
        searchEntry.set_text("");
        Refresh();
    }

    void PasswordStoreManager::Chdir(const PasswordStoreElements::PasswordStoreElement &elem) {
        passwordStore.Chdir(elem);
        searchEntry.set_text("");
        Refresh();
    }

    void PasswordStoreManager::Refresh() {
        auto filter = searchEntry.get_text();

        if (filter.empty())
            folderview.DisplayElements(passwordStore.GetElements());
        else
            folderview.DisplayElements(passwordStore.GetElements(filter));

        searchEntry.grab_focus();
        tools.SetGoUpButtonActive(!passwordStore.IsAtRoot());
        tools.SetPath(passwordStore.GetPath(true));
    }

    void PasswordStoreManager::PerformGitAction(GitManagerToolbar::Action action) {
        tools.PerformGitAction(action);
    }


}
