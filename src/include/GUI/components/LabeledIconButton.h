#pragma once

#include <GUI/components/PictureInsert.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <image/icons.h>
#include <string>

namespace Clavis::GUI {
	class LabeledIconButton : public Gtk::Button {
	public:
		LabeledIconButton();
		LabeledIconButton(Icons::IconDefinition icon, std::string label);

		void SetIcon(Icons::IconDefinition icon);
		void SetLabel(std::string label);

		void SetContentsHAlign(Gtk::Align align);
	protected:

	private:
		Gtk::Box MainHBox;
		Gtk::Label ButtonLabel;
		PictureInsert Icon;
	};
}