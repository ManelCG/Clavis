#include <GUI/palettes/first_run/ChoosePasswordStoreLocationPalette.h>

#include <language/Language.h>

#include <settings/Settings.h>
#include <system/Extensions.h>

namespace Clavis::GUI {
    ChoosePasswordStoreLocationPalette::ChoosePasswordStoreLocationPalette() :
        DualChoicePalette(_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_TITLE))
    {
        set_margin(10);
        set_default_size(420, -1);

        SetYesText(_(MISC_ACCEPT_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));

        choosePathLabel.set_text(_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_LABEL_TEXT));
        choosePathLabel.set_margin_bottom(5);

        choosePathEntry.set_text(Settings::PASSWORD_STORE_PATH.GetValue());

        okIcon.SetIcon(Icons::Check);
        alertIcon.SetIcon(Icons::Actions::Cancel);

        infoLabelHBox.append(okIcon);
        infoLabelHBox.append(alertIcon);
        infoLabelHBox.append(pathInfoLabel);
        infoLabelHBox.set_hexpand(true);
        infoLabelHBox.set_halign(Gtk::Align::CENTER);
        infoLabelHBox.set_margin_top(10);

        append(choosePathLabel);
        append(choosePathEntry);
        append(infoLabelHBox);

        choosePathEntry.signal_changed().connect([this]() {
            ProvidePathFeedback();
        });
        timeoutDispatcher.SetAction([this]() {
            choosePathEntry.remove_css_class("error");
        });
        ProvidePathFeedback();
    }

    std::filesystem::path ChoosePasswordStoreLocationPalette::GetSelectedPath() {
        return std::string(choosePathEntry.get_text());
    }

    void ChoosePasswordStoreLocationPalette::ProvidePathFeedback() {
        auto t = GetFolderType();

        switch (t) {
            case FolderType::FILE:
            case FolderType::NONEMPTY:
                okIcon.hide();
                alertIcon.show();
                break;
            case FolderType::NONEXISTANT:
            case FolderType::EMPTY:
                okIcon.show();
                alertIcon.hide();
                break;
        }

        switch (t) {
            case FolderType::FILE:
                pathInfoLabel.set_markup("<span color=\"red\">" + _(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_PATH_IS_FILE_INFO_LABEL) + "</span>");
                break;

            case FolderType::NONEXISTANT:
                pathInfoLabel.set_text(_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_FOLDER_DOES_NOT_EXIST_INFO_LABEL));
                break;

            case FolderType::NONEMPTY:
                pathInfoLabel.set_markup("<span color=\"red\">" + _(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_FOLDER_NOT_EMPTY_INFO_LABEL) + "</span>");
                break;

            case FolderType::EMPTY:
                pathInfoLabel.set_text(_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_FOLDER_EMPTY_INFO_LABEL));
                break;
        }
    }

    ChoosePasswordStoreLocationPalette::FolderType ChoosePasswordStoreLocationPalette::GetFolderType() {
        auto s = std::string(choosePathEntry.get_text());
        auto p = std::filesystem::path(s);

        if (System::FileExists(p))
            return FolderType::FILE;

        if (! System::DirectoryExists(p))
            return FolderType::NONEXISTANT;

        if (System::ListContents(p).empty())
            return FolderType::EMPTY;

        return FolderType::NONEMPTY;
    }

    void ChoosePasswordStoreLocationPalette::DoGiveResponse(bool r) {
        if (!r) {
            DualChoicePalette::DoGiveResponse(false);
            return;
        }

        auto t = GetFolderType();

        switch (t) {
            case FolderType::FILE:
            case FolderType::NONEMPTY:
                choosePathEntry.add_css_class("error");
                timeoutDispatcher.StartTimeout();
                return;

            case FolderType::EMPTY:
            case FolderType::NONEXISTANT:
                DualChoicePalette::DoGiveResponse(true);
                return;

            default:
                RaiseClavisError(_(ERROR_UNKNOWN_ENUM_VALUE, std::to_string(static_cast<int>(t))));
        }
    }




}