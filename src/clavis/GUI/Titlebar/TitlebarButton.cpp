#include <GUI/Titlebar/TitlebarButton.h>

#include <error/ClavisError.h>
#include <settings/Settings.h>

#include <image/icons.h>

namespace Clavis::GUI {
	TitlebarButton::TitlebarButton(ButtonAction action) {
		Action = action;

		std::string css = __STRINGIZE__(
			button{
				border-width: 0;
				border-radius: 0;
				box-shadow: none;
				margin: 0;
				padding: 0;
				background: linear-gradient(to right, transparent, transparent);
			}
		);

		if (action == Close) {
			css += __STRINGIZE__(
				button:hover{
					background: #E81123;
				}
			);
		}
		else {
			if (Settings::DO_USE_DARK_THEME.GetValue()) {
				css += __STRINGIZE__(
					button:hover{
						background: #3D3D3D;
					}
				);
			}
			else {
				css += __STRINGIZE__(
					button:hover{
						background: #E5E5E5;
					}
				);
			}
		}

		switch (action) {
		case Maximize:
			Insert.SetIcon(Icons::WindowControls::MaximizeWindow);

			signal_clicked().connect([this]() {
				auto maximized = ParentWindow->property_maximized().get_value();

				if (maximized)
					ParentWindow->unmaximize();
				else
					ParentWindow->maximize();
			});

			break;
		case Minimize:
			Insert.SetIcon(Icons::WindowControls::MinimizeWindow);

			signal_clicked().connect([this]() {
				ParentWindow->minimize();
			});

			break;
		case Close:
			Insert.SetIcon(Icons::WindowControls::CloseWindow);

			signal_clicked().connect([this]() {
				ParentWindow->close();
			});
			break;
		}

		Insert.set_can_shrink(false);
		Insert.set_halign(Gtk::Align::CENTER);
		Insert.set_valign(Gtk::Align::CENTER);
		set_child(Insert);

		Css = Gtk::CssProvider::create();
		Css->load_from_data(css.c_str());

		get_style_context()->add_provider(Css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	}

	void TitlebarButton::LinkWindow(Gtk::Window* w) {
		ParentWindow = w;

		if (Action == Maximize) {
			ParentWindow->property_maximized().signal_changed().connect([this]() {
				auto maximized = ParentWindow->property_maximized().get_value();

				if (maximized)
					Insert.SetIcon(Icons::WindowControls::UnMaximizeWindow);
				else
					Insert.SetIcon(Icons::WindowControls::MaximizeWindow);
			});
		}
	}
}