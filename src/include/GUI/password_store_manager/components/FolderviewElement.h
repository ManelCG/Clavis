#pragma once

#include <gtkmm.h>
#include <iostream>

#include <password_store/PasswordStoreElement.h>
#include <GUI/components/LabeledIconButton.h>

namespace Clavis::GUI {
    class FolderviewElement : public LabeledIconButton {
    public:
        // `insideWorkspace` changes what the context menu offers rather than what the item is:
        // the same password gets "Rename" in the tree and "Rename in Workspace" inside one, so
        // browsing a workspace can never rename a file on disk.
        explicit FolderviewElement(const PasswordStoreElements::PasswordStoreElement& element,
                                   bool insideWorkspace = false);

        void SetOnEditPassword(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnRenameItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnDeleteItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnExportFolder(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);

        void SetOnEditTwoFactor(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnShowTwoFactorDetails(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnTransferTwoFactor(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);

        void SetOnAddToWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnEditWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnRenameInWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnRemoveFromWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);

        PasswordStoreElements::PasswordStoreElement GetElement();

    protected:

    private:
        void SetupContextMenu();
        void SetupContextMenuGesture();
        void ResolveIcon();

        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> editPasswordCallback  = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> renameItemCallback   = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> deleteItemCallback   = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> exportFolderCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> editTwoFactorCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> showTwoFactorDetailsCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> transferTwoFactorCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> addToWorkspaceCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> editWorkspaceCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> renameInWorkspaceCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> removeFromWorkspaceCallback = [](const PasswordStoreElements::PasswordStoreElement&){};

        Icons::IconDefinition icon;

        PasswordStoreElements::PasswordStoreElement element;
        bool insideWorkspace;
        Glib::RefPtr<Gtk::GestureClick> clickGesture;

        // Popover
        Gtk::Box titleHBox;
        PictureInsert contextMenuIcon;
        Gtk::Label contextMenuFilenameLabel;
        LabeledIconButton editPasswordButton;
        LabeledIconButton exportFolderButton;
        LabeledIconButton editTwoFactorButton;
        LabeledIconButton showTwoFactorDetailsButton;
        LabeledIconButton transferTwoFactorButton;
        LabeledIconButton renameItemButton;
        LabeledIconButton deleteItemButton;
        LabeledIconButton addToWorkspaceButton;
        LabeledIconButton editWorkspaceButton;
        LabeledIconButton renameInWorkspaceButton;
        LabeledIconButton removeFromWorkspaceButton;
        Gtk::Box contextMenuVBox;
        Gtk::Popover contextMenu;
    };
}