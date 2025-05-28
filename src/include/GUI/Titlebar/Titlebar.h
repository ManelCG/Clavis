#pragma once

#include <GUI/Titlebar/TitlebarButton.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>

#include <gtkmm/label.h>
#include <gtkmm/window.h>
#include <gtkmm/cssprovider.h>

#include <string>
#include <memory>

namespace Clavis::GUI {
	class Titlebar :  public Gtk::HeaderBar{
	public:
		Titlebar();
		Titlebar(std::string title, bool canMinimize = true, bool canMaximize = true, bool canClose = true);

		void LinkWindow(Gtk::Window* w);
	protected:

	private:
		std::string Title;
		bool CanMinimize;
		bool CanMaximize;
		bool CanClose;

		PictureInsert Icon;
		Gtk::Label TitleLabel;
		TitlebarButton MinimizeButton;
		TitlebarButton MaximizeButton;
		TitlebarButton CloseButton;

		Glib::RefPtr<Gtk::CssProvider> CssBar;
		Glib::RefPtr<Gtk::CssProvider> CssBox;

		Gtk::Window* ParentWindow;
	};
}