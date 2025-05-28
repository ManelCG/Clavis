#include <GUI/Titlebar/Titlebar.h>

#include <iostream>

namespace Clavis::GUI {
	Titlebar::Titlebar() : Titlebar("", false, false, false) {}

	Titlebar::Titlebar(std::string title, bool canMinimize, bool canMaximize, bool canClose) :
		MinimizeButton(TitlebarButton::ButtonAction::Minimize),
		MaximizeButton(TitlebarButton::ButtonAction::Maximize),
		CloseButton(TitlebarButton::ButtonAction::Close)
	{
		Title = title;
		CanMinimize = canMinimize;
		CanMaximize = canMaximize;
		CanClose = canClose;

		set_margin(0);

		Icon.SetIcon(Icons::Logos::Logo32px);
		Icon.set_halign(Gtk::Align::START);
		Icon.set_valign(Gtk::Align::CENTER);
		Icon.set_can_shrink(false);
		Icon.set_margin(2);

		TitleLabel = Gtk::Label(Title);
		TitleLabel.set_margin_start(5);
		Gtk::Box dummy;
		set_title_widget(dummy);

		set_show_title_buttons(false);

		pack_start(Icon);
		pack_start(TitleLabel);

		Gtk::Box buttons(Gtk::Orientation::HORIZONTAL);
		buttons.append(MinimizeButton);
		buttons.append(MaximizeButton);
		buttons.append(CloseButton);
		pack_end(buttons);

		CssBar = Gtk::CssProvider::create();
		CssBar->load_from_data(__STRINGIZE__(
			headerbar{
				padding: 0;
				margin: 0;
				min-height: 0;
				background: linear-gradient(to right, transparent, transparent);
			}

			headerbar entry,
			headerbar spinbutton,
			headerbar button,
			headerbar separator {
				margin: 0;
				padding: 0;
			}

		));
		get_style_context()->add_provider(CssBar, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	}

	void Titlebar::LinkWindow(Gtk::Window* w) {
		ParentWindow = w;

		MinimizeButton.LinkWindow(ParentWindow);
		MaximizeButton.LinkWindow(ParentWindow);
		CloseButton.LinkWindow(ParentWindow);

		ParentWindow->property_transient_for().signal_changed().connect([this]() {
			if (ParentWindow->get_transient_for() != nullptr)
				MinimizeButton.hide();
			else
				MinimizeButton.show();
		});
		if (ParentWindow->get_transient_for() != nullptr)
			MinimizeButton.hide();

		ParentWindow->property_resizable().signal_changed().connect([this]() {
			if (ParentWindow->get_resizable())
				MaximizeButton.show();
			else
				MaximizeButton.hide();
		});
		if (!ParentWindow->get_resizable())
			MaximizeButton.hide();
	}
}