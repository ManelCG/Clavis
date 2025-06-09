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

        void DisplayElements(const std::vector<PasswordStoreElements::PasswordStoreElement>& elements);

        void ScrollUp();
        void ScrollDown();
        void ActivateFocusedItem();

        void SetOnEditPassword(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnRenameItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);
        void SetOnDeleteItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda);

    protected:

    private:
        void Clear();
        void SetFocusedItem(int i);

        int focusedItem;

        Gtk::Box mainVBox;

        std::vector<FolderviewElement*> folderviewElements;

        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> onElementClicked;

        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> editPasswordCallback = [](const PasswordStoreElements::PasswordStoreElement& elem){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> renameItemCallback = [](const PasswordStoreElements::PasswordStoreElement& elem){};
        std::function<void(const PasswordStoreElements::PasswordStoreElement&)> deleteItemCallback = [](const PasswordStoreElements::PasswordStoreElement& elem){};
    };
}