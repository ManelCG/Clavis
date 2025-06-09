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

    void Folderview::DisplayElements(const std::vector<PasswordStoreElements::PasswordStoreElement> &elements) {
        Clear();

        bool startedDrawing = false;
        auto currentType = PasswordStoreElements::PasswordStoreElementType::UNDEFINED;

        for (int i = 0; i < elements.size(); i++) {
            const auto& element = elements[i];

            if (startedDrawing && currentType != element.GetType()) {
                auto sep = Gtk::Separator(Gtk::Orientation::VERTICAL);
                sep.set_margin(5);
                mainVBox.append(sep);
            }

            currentType = element.GetType();
            startedDrawing = true;

            auto button = Gtk::make_managed<FolderviewElement>(element);

            button->signal_clicked().connect([this, button, i]() {
                if (const auto element = button->GetElement(); element.IsFolder()) {
                    onElementClicked(element);
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

            button->SetOnDeleteItem(deleteItemCallback);
            button->SetOnRenameItem(renameItemCallback);
            if (element.IsGPGFile())
                button->SetOnEditPassword(editPasswordCallback);

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
        if (folderviewElements.size() == 0)
            return;

        onElementClicked(folderviewElements[focusedItem]->GetElement());
    }



    void Folderview::SetOnElementClicked(const std::function<void(const PasswordStoreElements::PasswordStoreElement&)> &lambda) {
        onElementClicked = lambda;
    }

    void Folderview::Clear() {
        auto children = Extensions::GetAllChildren(&mainVBox);

        for (auto& elem : folderviewElements)
            mainVBox.remove(*elem);

        folderviewElements.clear();
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



}