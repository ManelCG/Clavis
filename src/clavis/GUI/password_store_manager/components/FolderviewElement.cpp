#include <GUI/password_store_manager/components/FolderviewElement.h>

#include <language/Language.h>

namespace Clavis::GUI {
    FolderviewElement::FolderviewElement(const PasswordStoreElements::PasswordStoreElement& element)
        :element(element)
    {
        ResolveIcon();

        SetLabel(element.GetName());

        SetIcon(icon);
        SetupContextMenu();
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
        contextMenuFilenameLabel.set_text(element.GetName());
        contextMenuFilenameLabel.set_margin_start(5);

        contextMenuVBox.append(titleHBox);

        Gtk::Separator sep(Gtk::Orientation::HORIZONTAL);
        contextMenuVBox.append(sep);

        if (element.IsGPGFile()) {
            contextMenuVBox.append(editPasswordButton);
            editPasswordButton.SetIcon(Icons::Placeholders::Placeholder16x16);
            editPasswordButton.SetLabel(_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_EDIT_PASSWORD_BUTTON));
            editPasswordButton.signal_clicked().connect([this]() {
                contextMenu.hide();
                editPasswordCallback(element);
            });
        }

        contextMenuVBox.append(renameItemButton);
        renameItemButton.SetIcon(Icons::Actions::Draw);
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