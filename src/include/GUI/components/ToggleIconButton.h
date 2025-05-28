#pragma once

#include <GUI/components/StateIconButton.h>
#include <gtkmm.h>

namespace Clavis::GUI {
    class ToggleIconButton : public IconButton {
    public:
        ToggleIconButton();
        ToggleIconButton(const Icons::IconDefinition &enabled, const Icons::IconDefinition &disabled, bool defaultEnabled);

        void SetIcons(const Icons::IconDefinition &enabled, const Icons::IconDefinition &disabled);
        void SetIconEnabled(const Icons::IconDefinition &icon);
        void SetIconDisabled(const Icons::IconDefinition &icon);

        bool GetState() const;
        void SetState(bool b);

    protected:

    private:
        Icons::IconDefinition iconEnabled;
        Icons::IconDefinition iconDisabled;

        bool state;
    };
}