#include <GUI/components/ToggleIconButton.h>

namespace Clavis::GUI {
    ToggleIconButton::ToggleIconButton() : iconEnabled(""), iconDisabled("")
     {
        iconEnabled = Icons::Placeholders::Placeholder16x16;
        iconDisabled = Icons::Placeholders::Placeholder16x16;

        SetState(false);

        signal_clicked().connect([this]() {
            SetState(!state);
        });
    }

    bool ToggleIconButton::GetState() const {
        return state;
    }


    ToggleIconButton::ToggleIconButton(const Icons::IconDefinition &enabled, const Icons::IconDefinition &disabled, const bool defaultEnabled) : ToggleIconButton(){
        SetIcons(enabled, disabled);

        SetState(defaultEnabled);
    }
    void ToggleIconButton::SetIcons(const Icons::IconDefinition &enabled, const Icons::IconDefinition &disabled) {
        SetIconEnabled(enabled);
        SetIconDisabled(disabled);
    }

    void ToggleIconButton::SetIconDisabled(const Icons::IconDefinition &icon) {
        iconDisabled = icon;

        if (!state)
            SetIcon(iconDisabled);
    }
    void ToggleIconButton::SetIconEnabled(const Icons::IconDefinition &icon) {
        iconEnabled = icon;

        if (state)
            SetIcon(iconEnabled);
    }


    void ToggleIconButton::SetState(bool b) {
        state = b;

        if (b) {
            SetIcon(iconEnabled);
            add_css_class("suggested-action");
        } else {
            SetIcon(iconDisabled);
            remove_css_class("suggested-action");
        }
    }


}