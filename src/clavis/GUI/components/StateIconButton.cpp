#include <GUI/components/StateIconButton.h>

namespace Clavis::GUI {
    StateIconButton::StateIconButton() {
        StyleContext = get_style_context();

        RegisterState(State::ERROR, "button {background: #910101;}");
        RegisterState(State::PROGRESS, "button {background: #00A5CD;}");
        RegisterState(State::SUCCESS, "button {background: #00BC06;}");
        RegisterState(State::WARNING, "button {background: #df9900;}");

        CurrentProvider = nullptr;
        DefaultTooltip = "";
    }

    void StateIconButton::RemoveState() {
        if (!HasState())
            return;

        ApplyTooltip(DefaultTooltip);
        SetIcon(DefaultIcon);

        StyleContext->remove_provider(CurrentProvider);
        CurrentProvider = nullptr;
    }

    void StateIconButton::ApplyState(State state) {
        RemoveState();

        auto provider = Providers[state];

        StyleContext->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        CurrentProvider = provider;

        CurrentState = state;

        Normalize();
    }

    void StateIconButton::SetDefaultTooltip(Glib::ustring tooltip) {
        DefaultTooltip = tooltip;

        NormalizeTooltip();
    }

    void StateIconButton::SetDefaultIcon(Icons::IconDefinition icon) {
        DefaultIcon = icon;

        NormalizeIcon();
    }


    void StateIconButton::RegisterState(State state, std::string css) {
        Providers[state] = Gtk::CssProvider::create();
        Providers[state]->load_from_data(css.c_str());
    }

    void StateIconButton::RegisterTooltip(State state, Glib::ustring tooltip) {
        Tooltips[state] = tooltip;
        NormalizeTooltip();
    }

    void StateIconButton::RegisterIcon(State state, Icons::IconDefinition icon) {
        Icons[state] = icon;
        NormalizeIcon();
    }



    void StateIconButton::ApplyTooltip(Glib::ustring tooltip) {
        set_tooltip_text(tooltip);

        if (tooltip == "")
            set_has_tooltip(false);
    }

    bool StateIconButton::HasState() {
        return CurrentProvider != nullptr;
    }

    void StateIconButton::NormalizeTooltip() {
        if (!HasState())
            ApplyTooltip(DefaultTooltip);
        else
            if (Tooltips.find(CurrentState) != Tooltips.end())
                ApplyTooltip(Tooltips[CurrentState]);
    }

    void StateIconButton::NormalizeIcon() {
        if (!HasState())
            SetIcon(DefaultIcon);
        else
            if (Icons.find(CurrentState) != Icons.end())
                SetIcon(Icons[CurrentState]);
    }

    void StateIconButton::Normalize() {
        NormalizeTooltip();
        NormalizeIcon();
    }

    StateIconButton::State StateIconButton::GetState() const {
        return CurrentState;
    }

    void StateIconButton::RegisterSetterID(int id) {
        stateSetterID = id;
    }
    int StateIconButton::GetSetterID() {
        return stateSetterID;
    }








}