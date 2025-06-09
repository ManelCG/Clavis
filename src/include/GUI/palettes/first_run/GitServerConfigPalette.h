#pragma once

#include <GUI/palettes/DualChoicePalette.h>

#include <GUI/components/TaskIconButton.h>
#include <GUI/components/RequiredEntry.h>

#include <gtkmm.h>

namespace Clavis::GUI {
    class GitServerConfigPalette : public DualChoicePalette<GitServerConfigPalette> {
    public:
        GitServerConfigPalette(const std::filesystem::path& repoPath);

    protected:

    private:
        std::filesystem::path passwordStoreLocation;

        Gtk::Label infoTitleLabel;
        Gtk::CheckButton doNotUseGitCheckButton;
        Gtk::CheckButton createANewRepoCheckButton;
        Gtk::CheckButton cloneAnExistingRepoCheckButton;


        // Repo tools
        Gtk::Box repoToolsVBox;
        Gtk::Label repoToolsTitleLabel;
        Gtk::Box repoToolsHBox;

        Gtk::Box repoToolsLabelsVBox;
        Gtk::Label repoToolsPathLabel;
        Gtk::Label repoToolsUpstreamLabel;

        Gtk::Box repoToolsEntriesVBox;
        Gtk::Entry repoToolsPathEntry;
        Gtk::Box repoToolsUpstreamHBox;
        RequiredEntry repoToolsUpstreamEntry;
        TaskIconButton newRepoButton;
        TaskIconButton cloneRepoButton;



        // Git user config
        Gtk::Box gitUserSettingsVBox;
        Gtk::Label gitUserSettingsLabel;
        Gtk::Box gitUserSettingsHBox;

        Gtk::Box gitUserSettingsLabelsVBox;
        Gtk::Label gitUserNameLabel;
        Gtk::Label gitUserEmailLabel;

        Gtk::Box gitUserSettingsEntriesVBox;
        RequiredEntry gitUserNameEntry;
        RequiredEntry gitUserEmailEntry;




        void DoGiveResponse(bool r) override;

        void InitRadioButtons();
        void PopulateWidgets();

        void InitLabelsVBox(Gtk::Box& box, std::vector<std::tuple<Gtk::Label&, std::string>> labels);
        void PopulateBoxes();
        void PopulateRepoToolsBox();
        void PopulateGitUserSettingsBox();
        void SetButtonCallbacks();

        bool IsRepoValid();
        bool IsGitUserSettingsValid();

        bool TryApplyGitUserSettings() const;

        void RefreshUI();
    };
}