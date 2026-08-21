#pragma once

#include <memory>
#include <vector>

#include <gtkmm.h>

#include <GUI/components/IconButton.h>
#include <GUI/components/PictureInsert.h>
#include <GUI/components/RequiredEntry.h>
#include <GUI/palettes/DualChoicePalette.h>

#include <password_store/Workspaces.h>

namespace Clavis::GUI {
    // The one dialog behind both "New Workspace" and "Edit Workspace". The two differ only in what
    // they start from -- an empty workspace or an existing one -- so splitting them into separate
    // dialogs would mean maintaining the same contents editor twice.
    class WorkspacePalette : public DualChoicePalette<WorkspacePalette> {
    public:
        explicit WorkspacePalette(const Workspaces::Workspace& workspace, bool isEditing);
        ~WorkspacePalette() override;

        [[nodiscard]] std::string GetWorkspaceName() const;

        // The rows as they stand when the dialog is confirmed: deleted rows are gone, and each
        // remaining name is whatever is in its entry.
        [[nodiscard]] std::vector<Workspaces::WorkspaceEntry> GetEntries() const;

    protected:
        // The name and alias fields both take Return as ordinary text input, so the dialog must
        // not swallow it to submit while one of them has focus.
        bool ShouldSubmitOnReturn() const override;

        void DoGiveResponse(bool r) override;

    private:
        struct Row {
            Gtk::Box box;
            PictureInsert icon;
            Gtk::Box textVBox;
            Gtk::Entry nameEntry;
            Gtk::Label pathLabel;
            IconButton deleteButton;

            Workspaces::WorkspaceEntry entry;
        };

        void BuildRow(const Workspaces::WorkspaceEntry& entry);
        void RemoveRow(Row* row);
        void UpdateEmptyHint();

        Gtk::Box mainVBox;
        Gtk::Label nameLabel;
        RequiredEntry nameEntry;

        Gtk::ScrolledWindow rowsScroll;
        Gtk::Box rowsVBox;
        Gtk::Label emptyHintLabel;

        std::vector<std::unique_ptr<Row>> rows;

        // Rows are destroyed from an idle callback rather than from inside their own delete
        // handler, which would free the widget mid-signal. The flag tells a queued callback that
        // the dialog went away first.
        std::shared_ptr<bool> alive;
    };
}
