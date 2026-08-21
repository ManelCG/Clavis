#include <GUI/password_store_manager/components/FolderviewElement.h>

#include <GUI/components/ElementIcon.h>
#include <language/Language.h>

namespace Clavis::GUI {
    FolderviewElement::FolderviewElement(const PasswordStoreElements::PasswordStoreElement& element,
                                         bool insideWorkspace)
        :element(element), insideWorkspace(insideWorkspace)
    {
        ResolveIcon();

        SetLabel(element.GetLabel());

        SetIcon(icon);

        // A workspace entry whose file is gone is still shown, in red: silently dropping it would
        // make the entry look like something the user removed themselves.
        if (element.IsMissing()) {
            add_css_class("error");
            set_tooltip_text(_(WORKSPACE_ELEMENT_MISSING_TOOLTIP));
        }

        SetupContextMenu();

        this->signal_unmap().connect([this]() {
            contextMenu.unparent();
        });

    }

    void FolderviewElement::ResolveIcon() {
        icon = ResolveIconForElement(element);
    }


    void FolderviewElement::SetupContextMenu() {
        contextMenu.set_parent(*this);

        contextMenu.set_child(contextMenuVBox);
        contextMenuVBox.set_orientation(Gtk::Orientation::VERTICAL);

        titleHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        titleHBox.append(contextMenuIcon);
        titleHBox.append(contextMenuFilenameLabel);
        titleHBox.set_hexpand(true);
        titleHBox.set_halign(Gtk::Align::CENTER);
        titleHBox.set_margin_top(5);
        titleHBox.set_margin_start(5);
        titleHBox.set_margin_end(5);

        contextMenuIcon.SetIcon(icon);
        contextMenuFilenameLabel.set_text(element.GetLabel());
        contextMenuFilenameLabel.set_margin_start(5);

        contextMenuVBox.append(titleHBox);

        Gtk::Separator sep(Gtk::Orientation::HORIZONTAL);
        contextMenuVBox.append(sep);

        // A workspace is not a file, so none of the filesystem actions below apply to it. It gets
        // its own three: edit its contents, rename or move it, and delete the workspace itself.
        if (element.IsWorkspace()) {
            contextMenuVBox.append(editWorkspaceButton);
            editWorkspaceButton.SetIcon(Icons::Actions::Build);
            editWorkspaceButton.SetLabel(_(WORKSPACE_CONTEXT_MENU_EDIT_WORKSPACE));
            editWorkspaceButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                editWorkspaceCallback(element);
            });

            contextMenuVBox.append(renameItemButton);
            renameItemButton.SetIcon(Icons::Actions::Rename);
            renameItemButton.SetLabel(_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_RENAME_ITEM_BUTTON));
            renameItemButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                renameItemCallback(element);
            });

            contextMenuVBox.append(deleteItemButton);
            deleteItemButton.SetIcon(Icons::Actions::Trash);
            deleteItemButton.SetLabel(_(MISC_DELETE_BUTTON));
            deleteItemButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                deleteItemCallback(element);
            });

            SetupContextMenuGesture();
            return;
        }

        // A missing target has no file to edit, rename or export. The only thing left to offer is
        // taking it out of the workspace.
        if (element.IsMissing()) {
            contextMenuVBox.append(removeFromWorkspaceButton);
            removeFromWorkspaceButton.SetIcon(Icons::Minus);
            removeFromWorkspaceButton.SetLabel(_(WORKSPACE_CONTEXT_MENU_REMOVE_FROM_WORKSPACE));
            removeFromWorkspaceButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                removeFromWorkspaceCallback(element);
            });

            SetupContextMenuGesture();
            return;
        }

        if (element.IsGPGFile()) {
            contextMenuVBox.append(editPasswordButton);
            editPasswordButton.SetIcon(Icons::Actions::Draw);
            editPasswordButton.SetLabel(_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_EDIT_PASSWORD_BUTTON));
            editPasswordButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                editPasswordCallback(element);
            });
        }

        if (element.IsTwoFactorFile()) {
            contextMenuVBox.append(editTwoFactorButton);
            editTwoFactorButton.SetIcon(Icons::Actions::Draw);
            editTwoFactorButton.SetLabel(_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_EDIT_TWO_FACTOR_BUTTON));
            editTwoFactorButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                editTwoFactorCallback(element);
            });

            contextMenuVBox.append(showTwoFactorDetailsButton);
            showTwoFactorDetailsButton.SetIcon(Icons::Actions::Search);
            showTwoFactorDetailsButton.SetLabel(_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_SHOW_DETAILS_BUTTON));
            showTwoFactorDetailsButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                showTwoFactorDetailsCallback(element);
            });

            contextMenuVBox.append(transferTwoFactorButton);
            transferTwoFactorButton.SetIcon(Icons::Actions::Export);
            transferTwoFactorButton.SetLabel(_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_TRANSFER_TWO_FACTOR_BUTTON));
            transferTwoFactorButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                transferTwoFactorCallback(element);
            });
        }

        if (element.IsFolder()) {
            contextMenuVBox.append(exportFolderButton);
            exportFolderButton.SetIcon(Icons::Actions::Export);
            exportFolderButton.SetLabel(_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_EXPORT_FOLDER_BUTTON));
            exportFolderButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                exportFolderCallback(element);
            });
        }

        // Both workspace actions take the same slot: with the other things you do *to* the entry,
        // above the rename/delete pair. Neither should trail after Delete, where a frequent,
        // harmless action sits below a destructive one and reads as an afterthought.
        if (insideWorkspace) {
            contextMenuVBox.append(removeFromWorkspaceButton);
            removeFromWorkspaceButton.SetIcon(Icons::Minus);
            removeFromWorkspaceButton.SetLabel(_(WORKSPACE_CONTEXT_MENU_REMOVE_FROM_WORKSPACE));
            removeFromWorkspaceButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                removeFromWorkspaceCallback(element);
            });
        }
        // Folders cannot be put in a workspace: a workspace is a flat, curated list of entries,
        // so there is nothing sensible to show when you open one that contains a directory.
        else if (!element.IsFolder()) {
            contextMenuVBox.append(addToWorkspaceButton);
            addToWorkspaceButton.SetIcon(Icons::Actions::Build);
            addToWorkspaceButton.SetLabel(_(WORKSPACE_CONTEXT_MENU_ADD_TO_WORKSPACE));
            addToWorkspaceButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                addToWorkspaceCallback(element);
            });
        }

        // Inside a workspace, renaming acts on the workspace's own label for the file. Renaming
        // the file on disk from here would be a surprise -- the name shown is not the filename.
        if (insideWorkspace) {
            contextMenuVBox.append(renameInWorkspaceButton);
            renameInWorkspaceButton.SetIcon(Icons::Actions::Rename);
            renameInWorkspaceButton.SetLabel(_(WORKSPACE_CONTEXT_MENU_RENAME_IN_WORKSPACE));
            renameInWorkspaceButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                renameInWorkspaceCallback(element);
            });
        } else {
            contextMenuVBox.append(renameItemButton);
            renameItemButton.SetIcon(Icons::Actions::Rename);
            renameItemButton.SetLabel(_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_RENAME_ITEM_BUTTON));
            renameItemButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                renameItemCallback(element);
            });
        }

        contextMenuVBox.append(deleteItemButton);
        deleteItemButton.SetIcon(Icons::Actions::Trash);
        deleteItemButton.SetLabel(_(MISC_DELETE_BUTTON));
        deleteItemButton.signal_clicked().connect([this]() {
            contextMenu.hide();
            deleteItemCallback(element);
        });

        SetupContextMenuGesture();
    }

    void FolderviewElement::SetupContextMenuGesture() {
        clickGesture = Gtk::GestureClick::create();
        clickGesture->set_button(GDK_BUTTON_SECONDARY);
        clickGesture->signal_pressed().connect([this](int npress, double x, double y) {
            contextMenu.popup();
        });
        add_controller(clickGesture);
    }


    void FolderviewElement::SetOnDeleteItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        deleteItemCallback = lambda;
    }
    void FolderviewElement::SetOnExportFolder(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        exportFolderCallback = lambda;
    }
    void FolderviewElement::SetOnEditTwoFactor(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        editTwoFactorCallback = lambda;
    }
    void FolderviewElement::SetOnShowTwoFactorDetails(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        showTwoFactorDetailsCallback = lambda;
    }
    void FolderviewElement::SetOnTransferTwoFactor(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        transferTwoFactorCallback = lambda;
    }
    void FolderviewElement::SetOnEditPassword(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        editPasswordCallback = lambda;
    }
    void FolderviewElement::SetOnRenameItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        renameItemCallback = lambda;
    }
    void FolderviewElement::SetOnAddToWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        addToWorkspaceCallback = lambda;
    }
    void FolderviewElement::SetOnEditWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        editWorkspaceCallback = lambda;
    }
    void FolderviewElement::SetOnRenameInWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        renameInWorkspaceCallback = lambda;
    }
    void FolderviewElement::SetOnRemoveFromWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        removeFromWorkspaceCallback = lambda;
    }

    PasswordStoreElements::PasswordStoreElement FolderviewElement::GetElement() {
        return element;
    }
}