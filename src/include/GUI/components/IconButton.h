#pragma once

#include <GUI/components/PictureInsert.h>
#include <gtkmm/button.h>
#include <image/icons.h>

namespace Clavis::GUI {
	class IconButton : public Gtk::Button {
	public:
		IconButton();
		IconButton(Icons::IconDefinition icon);

		void SetIcon(Icons::IconDefinition icon);

	protected:

	private:
		PictureInsert Icon;
	};
}