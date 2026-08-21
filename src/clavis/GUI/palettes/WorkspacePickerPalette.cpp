#include <GUI/palettes/WorkspacePickerPalette.h>

#include <language/Language.h>

namespace Clavis::GUI {
    namespace {
        constexpr int ROWS_MIN_HEIGHT = 200;
        constexpr int PROMPT_MAX_WIDTH_CHARS = 34;
    }

    WorkspacePickerPalette::WorkspacePickerPalette(const std::vector<Workspaces::Workspace>& workspaces,
                                                   const std::string& elementLabel)
        : DualChoicePalette(_(WORKSPACE_PICKER_TITLE)),
          mainVBox(Gtk::Orientation::VERTICAL),
          workspaces(workspaces)
    {
        set_title(_(WORKSPACE_PICKER_TITLE));

        promptLabel.set_text(workspaces.empty()
            ? _(WORKSPACE_PICKER_EMPTY)
            : _(WORKSPACE_PICKER_LABEL, elementLabel));
        promptLabel.set_wrap(true);
        promptLabel.set_max_width_chars(PROMPT_MAX_WIDTH_CHARS);
        promptLabel.set_xalign(0.0f);
        promptLabel.set_margin_bottom(10);

        mainVBox.append(promptLabel);

        rowsVBox.set_orientation(Gtk::Orientation::VERTICAL);
        rowsScroll.set_child(rowsVBox);
        rowsScroll.set_vexpand(true);
        rowsScroll.property_hscrollbar_policy().set_value(Gtk::PolicyType::NEVER);

        if (!workspaces.empty()) {
            rowsScroll.set_min_content_height(ROWS_MIN_HEIGHT);
            mainVBox.append(rowsScroll);
        }

        append(mainVBox);
        set_margin(10);

        for (const auto& workspace : this->workspaces)
            BuildRow(workspace);

        // Choosing happens by clicking a workspace, so the confirm button would be a second way to
        // do nothing. Only the cancel button is meaningful here.
        SetYesEnabled(false);
    }

    void WorkspacePickerPalette::BuildRow(const Workspaces::Workspace& workspace) {
        auto row = std::make_unique<LabeledIconButton>();

        row->SetIcon(Icons::Actions::Build);
        row->SetLabel(workspace.name);
        row->SetContentsHAlign(Gtk::Align::START);
        row->set_margin_bottom(4);

        row->signal_clicked().connect([this, workspace]() {
            chosen = workspace;
            DoGiveResponse(true);
        });

        rowsVBox.append(*row);
        rows.push_back(std::move(row));
    }

    std::optional<Workspaces::Workspace> WorkspacePickerPalette::GetChosen() const {
        return chosen;
    }
}
