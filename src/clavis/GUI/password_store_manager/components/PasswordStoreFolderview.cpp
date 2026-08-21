#include <GUI/password_store_manager/components/PasswordStoreFolderview.h>

#include <extensions/GUIExtensions.h>

#include <gtkmm.h>

namespace Clavis::GUI {

    Folderview::Folderview() : Gtk::ScrolledWindow(),
        mainVBox(Gtk::Orientation::VERTICAL)
    {
        set_margin(10);

        set_child(mainVBox);
        mainVBox.set_vexpand(true);

        focusedItem = 0;
    }

    void Folderview::DisplayElements(const std::vector<PasswordStoreElements::PasswordStoreElement> &elements,
                                     bool insideWorkspace) {
        Clear();

        bool startedDrawing = false;
        auto currentType = PasswordStoreElements::PasswordStoreElementType::UNDEFINED;

        for (int i = 0; i < elements.size(); i++) {
            const auto& element = elements[i];

            if (startedDrawing && currentType != element.GetType()) {
                auto sep = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::VERTICAL);
                sep->set_margin(5);
                mainVBox.append(*sep);
                typeSeparators.push_back(sep);
            }

            currentType = element.GetType();
            startedDrawing = true;

            auto button = Gtk::make_managed<FolderviewElement>(element, insideWorkspace);

            button->signal_clicked().connect([this, button, i]() {
                const auto clicked = button->GetElement();

                // Folders and workspaces navigate, and a missing entry opens a prompt instead of
                // decrypting. None of those need the delay below, which only exists so the focus
                // highlight paints before GPG blocks the main loop.
                if (clicked.IsFolder() || clicked.IsWorkspace() || clicked.IsMissing()) {
                    onElementClicked(clicked);
                    return;
                }

                // This is a password. It looks nicer if we wait a few milliseconds
                // Before decrypting, so GTK refreshes, and we change the focused item.
                SetFocusedItem(i);

                Glib::signal_timeout().connect_once(
                    [this, button]() {
                        onElementClicked(button->GetElement());
                    },
                    75
                );
            });

            if (element.IsWorkspace()) {
                button->SetOnEditWorkspace(editWorkspaceCallback);
                button->SetOnRenameItem(renameItemCallback);
                button->SetOnDeleteItem(deleteItemCallback);
            } else {
                button->SetOnDeleteItem(deleteItemCallback);

                if (insideWorkspace) {
                    button->SetOnRenameInWorkspace(renameInWorkspaceCallback);
                    button->SetOnRemoveFromWorkspace(removeFromWorkspaceCallback);
                } else {
                    button->SetOnRenameItem(renameItemCallback);
                    button->SetOnAddToWorkspace(addToWorkspaceCallback);
                }

                if (element.IsGPGFile())
                    button->SetOnEditPassword(editPasswordCallback);
                if (element.IsFolder())
                    button->SetOnExportFolder(exportFolderCallback);
                if (element.IsTwoFactorFile()) {
                    button->SetOnEditTwoFactor(editTwoFactorCallback);
                    button->SetOnShowTwoFactorDetails(showTwoFactorDetailsCallback);
                    button->SetOnTransferTwoFactor(transferTwoFactorCallback);
                }
            }

            mainVBox.append(*button);

            folderviewElements.push_back(button);
        }

        SetFocusedItem(0);
    }

    void Folderview::SetFocusedItem(int i) {
        const auto N = folderviewElements.size();

        if (N == 0) {
            focusedItem = 0;
            return;
        }

        i = (i + N) % N;

        const auto previousFocused = focusedItem;
        focusedItem = i;

        if (previousFocused < N)
            folderviewElements[previousFocused]->remove_css_class("suggested-action");

        folderviewElements[focusedItem]->add_css_class("suggested-action");
    }

    void Folderview::ScrollDown() {
        SetFocusedItem(focusedItem + 1);
    }

    void Folderview::ScrollUp() {
        SetFocusedItem(focusedItem - 1);
    }

    void Folderview::ActivateFocusedItem() {
        if (folderviewElements.empty())
            return;

        onElementClicked(folderviewElements[focusedItem]->GetElement());
    }



    void Folderview::SetOnElementClicked(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda) {
        onElementClicked = lambda;
    }

    void Folderview::Clear() {
        for (auto& elem : folderviewElements)
            mainVBox.remove(*elem);

        for (auto sep : typeSeparators)
            mainVBox.remove(*sep);

        folderviewElements.clear();
        typeSeparators.clear();
    }

    void Folderview::SetOnDeleteItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        deleteItemCallback = lambda;
    }
    void Folderview::SetOnEditPassword(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        editPasswordCallback = lambda;
    }
    void Folderview::SetOnRenameItem(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        renameItemCallback = lambda;
    }
    void Folderview::SetOnExportFolder(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        exportFolderCallback = lambda;
    }
    void Folderview::SetOnEditTwoFactor(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        editTwoFactorCallback = lambda;
    }
    void Folderview::SetOnShowTwoFactorDetails(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        showTwoFactorDetailsCallback = lambda;
    }
    void Folderview::SetOnTransferTwoFactor(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        transferTwoFactorCallback = lambda;
    }
    void Folderview::SetOnAddToWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        addToWorkspaceCallback = lambda;
    }
    void Folderview::SetOnEditWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        editWorkspaceCallback = lambda;
    }
    void Folderview::SetOnRenameInWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        renameInWorkspaceCallback = lambda;
    }
    void Folderview::SetOnRemoveFromWorkspace(const std::function<void(const PasswordStoreElements::PasswordStoreElement &)> &lambda) {
        removeFromWorkspaceCallback = lambda;
    }



}