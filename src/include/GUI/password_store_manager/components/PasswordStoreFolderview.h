#pragma once

#include <gtkmm.h>

#include <vector>

#include <password_store/PasswordStoreElement.h>

#include <GUI/password_store_manager/components/FolderviewElement.h>

namespace Clavis::GUI {
    class Folderview : public Gtk::ScrolledWindow {
    public:
        Folderview();

        void SetOnElementClicked(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);

        // `insideWorkspace` is a property of the view, not of the elements: the same password
        // element is drawn differently depending on whether it was reached through the tree or
        // through a workspace.
        void DisplayElements(const std::vector<PasswordStoreElements::PasswordStoreElement>& elements,
                             bool insideWorkspace = false);

        void ScrollUp();
        void ScrollDown();
        void ActivateFocusedItem();

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

    protected:

    private:
        void Clear();
        void SetFocusedItem(int i);

        int focusedItem;

        std::vector<Gtk::Separator*> typeSeparators;

        Gtk::Box mainVBox;

        std::vector<FolderviewElement*> folderviewElements;

        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> onElementClicked;

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
    };
}