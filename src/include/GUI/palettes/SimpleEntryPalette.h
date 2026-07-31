#pragma once

#include <GUI/palettes/DualChoicePalette.h>

#include <GUI/components/RequiredEntry.h>

#include <gtkmm.h>

namespace Clavis::GUI {
    class SimpleEntryPalette : public DualChoicePalette<SimpleEntryPalette>{
    public:
        SimpleEntryPalette();

        std::string GetEntryText() const;
        void SetEntryText(const std::string& text);
        void SetTitle(const std::string& title);
        void SetLabelText(const std::string& text);

        void SetIsEntryRequired(bool isRequired);
        void SetIsEntryRequiredForYes(bool isRequired);
        void SetIsEntryRequiredForNo(bool isRequired);

    protected:

    private:
    private:
        Gtk::Box mainVBox;
        Gtk::Box mainHBox;

        Gtk::Label titleLabel;
        RequiredEntry entry;

        bool isEntryRequiredForYes = false;
        bool isEntryRequiredForNo = false;

        void DoGiveResponse(bool r) override;
    };
}