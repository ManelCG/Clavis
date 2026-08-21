#include <GUI/palettes/WorkspacePalette.h>

#include <algorithm>

#include <GUI/components/ElementIcon.h>
#include <extensions/StringHelper.h>
#include <language/Language.h>
#include <system/Extensions.h>

namespace Clavis::GUI {
    namespace {
        constexpr int ROWS_MIN_HEIGHT = 260;
        constexpr int NAME_ENTRY_WIDTH_CHARS = 28;
        constexpr int ICON_SIZE = 16;

        // Entry names and store paths are both long and both matter, so this dialog needs far
        // more width than the yes/no palettes it inherits its defaults from.
        constexpr int DEFAULT_WIDTH = 620;
        constexpr int DEFAULT_HEIGHT = 460;
    }

    WorkspacePalette::WorkspacePalette(const Workspaces::Workspace& workspace, bool isEditing)
        : DualChoicePalette(isEditing ? _(WORKSPACE_PALETTE_EDIT_TITLE) : _(WORKSPACE_PALETTE_NEW_TITLE)),
          mainVBox(Gtk::Orientation::VERTICAL),
          alive(std::make_shared<bool>(true))
    {
        set_title(isEditing ? _(WORKSPACE_PALETTE_EDIT_TITLE) : _(WORKSPACE_PALETTE_NEW_TITLE));

        nameLabel.set_text(_(WORKSPACE_PALETTE_NAME_LABEL));
        nameLabel.set_xalign(0.0f);
        nameLabel.set_margin_bottom(5);

        nameEntry.set_text(workspace.name);
        nameEntry.set_width_chars(NAME_ENTRY_WIDTH_CHARS);
        nameEntry.set_hexpand(true);

        mainVBox.append(nameLabel);
        mainVBox.append(nameEntry);

        rowsVBox.set_orientation(Gtk::Orientation::VERTICAL);
        rowsScroll.set_child(rowsVBox);
        rowsScroll.set_min_content_height(ROWS_MIN_HEIGHT);
        rowsScroll.set_vexpand(true);
        rowsScroll.set_margin_top(10);
        rowsScroll.property_hscrollbar_policy().set_value(Gtk::PolicyType::NEVER);

        emptyHintLabel.set_text(_(WORKSPACE_PALETTE_EMPTY_HINT));
        emptyHintLabel.set_wrap(true);
        emptyHintLabel.set_max_width_chars(NAME_ENTRY_WIDTH_CHARS);
        emptyHintLabel.set_margin(10);
        emptyHintLabel.add_css_class("dim-label");
        rowsVBox.append(emptyHintLabel);

        mainVBox.append(rowsScroll);
        append(mainVBox);

        set_margin(10);

        // DualChoicePalette fixes its size, which suits a one-line question but not a list the
        // user is editing: how much of it fits on screen should be theirs to decide.
        set_resizable(true);
        set_default_size(DEFAULT_WIDTH, DEFAULT_HEIGHT);

        for (const auto& entry : workspace.entries)
            BuildRow(entry);

        UpdateEmptyHint();

        nameEntry.grab_focus();
    }

    WorkspacePalette::~WorkspacePalette() {
        *alive = false;
    }

    void WorkspacePalette::BuildRow(const Workspaces::WorkspaceEntry& entry) {
        auto row = std::make_unique<Row>();
        row->entry = entry;

        row->box.set_orientation(Gtk::Orientation::HORIZONTAL);
        row->box.set_margin(2);

        // The row's icon has to agree with what the folderview draws for the same file, including
        // the plain-document fallback for anything Clavis does not recognise.
        const auto fullpath = System::GetPasswordStoreFolder() / entry.target;
        const auto element = System::FileExists(fullpath)
            ? PasswordStoreElements::PasswordStoreElement(fullpath)
            : PasswordStoreElements::PasswordStoreElement::MakeMissing(fullpath);

        row->icon.SetIcon(ResolveIconForElement(element));
        row->icon.Resize(ICON_SIZE, ICON_SIZE);
        row->icon.set_valign(Gtk::Align::CENTER);
        row->icon.set_margin_end(8);

        row->textVBox.set_orientation(Gtk::Orientation::VERTICAL);
        row->textVBox.set_hexpand(true);

        row->nameEntry.set_text(entry.name);
        row->nameEntry.set_hexpand(true);

        // The path is what the name actually points at, so it stays visible -- but quieter than
        // the name, which is the thing being edited.
        row->pathLabel.set_text(entry.target.generic_string());
        row->pathLabel.set_xalign(0.0f);
        row->pathLabel.set_margin_start(4);
        row->pathLabel.set_ellipsize(Pango::EllipsizeMode::MIDDLE);
        row->pathLabel.add_css_class("dim-label");
        row->pathLabel.add_css_class("caption");

        if (element.IsMissing()) {
            row->pathLabel.add_css_class("error");
            row->pathLabel.set_tooltip_text(_(WORKSPACE_ELEMENT_MISSING_TOOLTIP));
        }

        row->textVBox.append(row->nameEntry);
        row->textVBox.append(row->pathLabel);

        row->deleteButton.SetIcon(Icons::Actions::Trash);
        row->deleteButton.set_tooltip_text(_(WORKSPACE_CONTEXT_MENU_REMOVE_FROM_WORKSPACE));
        row->deleteButton.set_margin_start(5);
        row->deleteButton.set_valign(Gtk::Align::CENTER);

        row->box.append(row->icon);
        row->box.append(row->textVBox);
        row->box.append(row->deleteButton);

        auto* rowPtr = row.get();
        auto aliveFlag = alive;

        row->deleteButton.signal_clicked().connect([this, rowPtr, aliveFlag]() {
            // Deferred: destroying the row here would free the button while its own handler is
            // still on the stack.
            Glib::signal_idle().connect_once([this, rowPtr, aliveFlag]() {
                if (!*aliveFlag)
                    return;

                RemoveRow(rowPtr);
            });
        });

        rowsVBox.append(row->box);
        rows.push_back(std::move(row));
    }

    void WorkspacePalette::RemoveRow(Row* row) {
        const auto it = std::find_if(rows.begin(), rows.end(),
                                     [row](const auto& candidate) { return candidate.get() == row; });

        if (it == rows.end())
            return;

        // Focus has to leave the row before it dies, or the ScrolledWindow chases whatever GTK
        // hands focus to next and jumps the viewport.
        nameEntry.grab_focus();

        rowsVBox.remove((*it)->box);
        rows.erase(it);

        UpdateEmptyHint();
    }

    void WorkspacePalette::UpdateEmptyHint() {
        emptyHintLabel.set_visible(rows.empty());
    }

    std::string WorkspacePalette::GetWorkspaceName() const {
        return StringHelper::Trim(nameEntry.get_text());
    }

    std::vector<Workspaces::WorkspaceEntry> WorkspacePalette::GetEntries() const {
        std::vector<Workspaces::WorkspaceEntry> ret;
        ret.reserve(rows.size());

        for (const auto& row : rows) {
            Workspaces::WorkspaceEntry entry = row->entry;
            entry.name = StringHelper::Trim(row->nameEntry.get_text());

            // An emptied name is a blank row, not a request to have no name: fall back to what the
            // file is called rather than rejecting the whole dialog over it.
            if (entry.name.empty())
                entry.name = entry.target.filename().string();

            ret.push_back(entry);
        }

        return ret;
    }

    bool WorkspacePalette::ShouldSubmitOnReturn() const {
        if (nameEntry.has_focus())
            return false;

        for (const auto& row : rows)
            if (row->nameEntry.has_focus())
                return false;

        return true;
    }

    void WorkspacePalette::DoGiveResponse(bool r) {
        if (r && GetWorkspaceName().empty()) {
            nameEntry.DisplayError();
            nameEntry.grab_focus();
            return;
        }

        DualChoicePalette::DoGiveResponse(r);
    }
}
