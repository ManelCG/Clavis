#pragma once

#include <gtkmm.h>
#include <iostream>

#include <password_store/PasswordStoreElement.h>
#include <GUI/components/LabeledIconButton.h>

namespace Clavis::GUI {
    class FolderviewElement : public LabeledIconButton {
    public:
        explicit FolderviewElement(const PasswordStoreElements::PasswordStoreElement& element);

        void SetOnEditPassword(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnRenameItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnDeleteItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnExportFolder(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);

        void SetOnEditTwoFactor(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnShowTwoFactorDetails(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnTransferTwoFactor(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);

        PasswordStoreElements::PasswordStoreElement GetElement();

    protected:

    private:
        void SetupContextMenu();
        void ResolveIcon();

        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> editPasswordCallback  = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> renameItemCallback   = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> deleteItemCallback   = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> exportFolderCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> editTwoFactorCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> showTwoFactorDetailsCallback = [](const PasswordStoreElements::PasswordStoreElement&){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> transferTwoFactorCallback = [](const PasswordStoreElements::PasswordStoreElement&){};

        Icons::IconDefinition icon;

        PasswordStoreElements::PasswordStoreElement element;
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
        Gtk::Box contextMenuVBox;
        Gtk::Popover contextMenu;
    };
}