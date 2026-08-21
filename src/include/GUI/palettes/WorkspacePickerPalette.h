#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <gtkmm.h>

#include <GUI/components/LabeledIconButton.h>
#include <GUI/palettes/DualChoicePalette.h>

#include <password_store/Workspaces.h>

namespace Clavis::GUI {
    // "Which workspace?" -- a plain list of every workspace in the store, one button each. Names
    // alone: the folder a workspace lives in is not what you pick it by, and a second line of
    // path under every row turns a two-second choice into a wall of text.
    class WorkspacePickerPalette : public DualChoicePalette<WorkspacePickerPalette> {
    public:
        WorkspacePickerPalette(const std::vector<Workspaces::Workspace>& workspaces,
                               const std::string& elementLabel);

        // Empty when the dialog was dismissed without choosing.
        [[nodiscard]] std::optional<Workspaces::Workspace> GetChosen() const;

    private:
        void BuildRow(const Workspaces::Workspace& workspace);

        Gtk::Box mainVBox;
        Gtk::Label promptLabel;
        Gtk::ScrolledWindow rowsScroll;
        Gtk::Box rowsVBox;

        std::vector<std::unique_ptr<LabeledIconButton>> rows;
        std::vector<Workspaces::Workspace> workspaces;

        std::optional<Workspaces::Workspace> chosen;
    };
}
