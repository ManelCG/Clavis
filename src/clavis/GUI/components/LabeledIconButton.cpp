#include <GUI/components/LabeledIconButton.h>

namespace Clavis::GUI {
	LabeledIconButton::LabeledIconButton() {
		MainHBox = Gtk::Box(Gtk::Orientation::HORIZONTAL);
		set_child(MainHBox);

		MainHBox.append(Icon);
		MainHBox.append(ButtonLabel);

		ButtonLabel.set_margin_start(5);

		Icon.set_valign(Gtk::Align::CENTER);
		Icon.set_halign(Gtk::Align::CENTER);
		Icon.set_hexpand(false);
		Icon.set_vexpand(false);
		Icon.set_can_shrink(false);

	}
	LabeledIconButton::LabeledIconButton(Icons::IconDefinition icon, std::string label) :
		LabeledIconButton()
	{
		SetIcon(icon);
		SetLabel(label);
	}

	void LabeledIconButton::SetLabel(std::string label) {
		ButtonLabel.set_text(label);
	}

	void LabeledIconButton::SetIcon(Icons::IconDefinition icon) {
		Icon.SetIcon(icon);
	}

	void LabeledIconButton::SetContentsHAlign(Gtk::Align align) {
		MainHBox.set_halign(align);
	}

}