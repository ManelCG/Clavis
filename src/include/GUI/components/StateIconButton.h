#pragma once

#include <GUI/components/IconButton.h>
#include <gtkmm.h>

namespace Clavis::GUI {
    class StateIconButton : public IconButton {
    public:
        enum class State {
            PROGRESS,
            SUCCESS,
            WARNING,
            ERROR
        };

        StateIconButton();

        void RemoveState();
        void ApplyState(State state);

        void ApplyTooltip(Glib::ustring tooltip);

        void SetDefaultTooltip(Glib::ustring tooltip);
        void RegisterTooltip(State state, Glib::ustring tooltip);
        void SetDefaultIcon(Icons::IconDefinition icon);
        void RegisterIcon(State state, Icons::IconDefinition icon);

        void RegisterSetterID(int id);
        int GetSetterID();

        State GetState() const;

    private:
        Glib::RefPtr<Gtk::StyleContext> StyleContext;

        std::map<State, Glib::RefPtr<Gtk::CssProvider>> Providers;
        std::map<State, Glib::ustring> Tooltips;
        std::map<State, Icons::IconDefinition> Icons;

        Glib::ustring DefaultTooltip;
        Icons::IconDefinition DefaultIcon;

        State CurrentState;

        Glib::RefPtr<Gtk::CssProvider> CurrentProvider;

        int stateSetterID;


        void RegisterState(State state, std::string css);

        void NormalizeTooltip();
        void NormalizeIcon();
        void Normalize();
        bool HasState();

    };
}