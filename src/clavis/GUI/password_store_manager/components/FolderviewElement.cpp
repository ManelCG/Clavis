#include <GUI/password_store_manager/components/FolderviewElement.h>

#include <language/Language.h>

namespace Clavis::GUI {
    FolderviewElement::FolderviewElement(const PasswordStoreElements::PasswordStoreElement& element)
        :element(element)
    {
        ResolveIcon();

        SetLabel(element.GetLabel());

        SetIcon(icon);
        SetupContextMenu();

        this->signal_unmap().connect([this]() {
            contextMenu.unparent();
        });

    }

    void FolderviewElement::ResolveIcon() {
        if (element.IsFolder()) {
            icon = Icons::Actions::Folder;
            return;
        }

        if (element.IsGPGFile()) {
            icon = Icons::Actions::Password;
            return;
        }

        if (element.IsTwoFactorFile()) {
            icon = Icons::TwoFactor;
            return;
        }

        icon = Icons::Actions::Document;
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

    PasswordStoreElements::PasswordStoreElement FolderviewElement::GetElement() {
        return element;
    }
}