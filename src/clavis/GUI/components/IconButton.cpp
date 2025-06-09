#include <GUI/components/IconButton.h>

#include <extensions/GUIExtensions.h>

namespace Clavis::GUI {
	IconButton::IconButton()
	{
		set_child(Icon);


        property_sensitive().signal_changed().connect([this](){
            if (property_sensitive().get_value())
                Extensions::ApplyCss(&Icon, "*{opacity: 1;}");
            else
                Extensions::ApplyCss(&Icon, "*{opacity: 0.4;}");
        });
	}

	IconButton::IconButton(Icons::IconDefinition icon) : IconButton() {
		SetIcon(icon);
	}

	void IconButton::SetIcon(Icons::IconDefinition icon) {
		Icon.SetIcon(icon);
	}
}