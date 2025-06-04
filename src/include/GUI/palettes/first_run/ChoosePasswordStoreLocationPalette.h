#pragma once

#include <filesystem>

#include <GUI/palettes/DualChoicePalette.h>
#include <GUI/signals/UniqueSignalTimeoutDispatcher.h>

namespace Clavis::GUI {
    class ChoosePasswordStoreLocationPalette : public DualChoicePalette<ChoosePasswordStoreLocationPalette> {
    public:
        ChoosePasswordStoreLocationPalette();

        std::filesystem::path GetSelectedPath();

    protected:

    private:
        enum class FolderType {
            NONEXISTANT,
            EMPTY,
            NONEMPTY,
            FILE
        };

        Gtk::Label choosePathLabel;

        Gtk::Box pathHBox;
        Gtk::Entry choosePathEntry;
        LabeledIconButton browseButton;


        Gtk::Box infoLabelHBox;
        PictureInsert okIcon;
        PictureInsert alertIcon;
        Gtk::Label pathInfoLabel;

        UniqueSignalTimeoutDispatcher timeoutDispatcher;

        void DoGiveResponse(bool r) override;

        void ProvidePathFeedback();
        FolderType GetFolderType();
    };
}