#pragma once

#include <gtkmm/button.h>
#include <gtkmm/cssprovider.h>

#include <GUI/components/PictureInsert.h>

#include <string>

#include <gtkmm/window.h>

namespace Clavis::GUI {
	class TitlebarButton : public Gtk::Button {
	public:
		enum ButtonAction {
			Minimize,
			Maximize,
			Close,
		};

		TitlebarButton(ButtonAction action);
		void LinkWindow(Gtk::Window* w);

	protected:

	private:
		Glib::RefPtr<Gtk::CssProvider> Css;

		ButtonAction Action;
		PictureInsert Insert;

		Gtk::Window* ParentWindow = nullptr;
	};
}