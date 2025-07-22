#include <GUI/palettes/Palette.h>

#include <gtkmm.h>

#include <settings/Settings.h>

#include <extensions/GUIExtensions.h>

namespace Clavis {
	namespace GUI {
		Palette::Palette(Glib::ustring title) {
			set_hide_on_close(true);
			set_title(title);

			if (Settings::WINDOW_DECORATIONS.GetValue() == Settings::WINDOW_DECORATIONS.CLAVIS_CSD)
				SetCustomTitlebar();

			if (Settings::DISABLE_SHADOWS.GetValue())
				DisableShadows();

			const auto key_controller = Gtk::EventControllerKey::create();
			key_controller->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
			key_controller->signal_key_pressed().connect(
				sigc::mem_fun(*this, &Palette::on_key_pressed), false);

			add_controller(key_controller);  // Attach to the entry

		}

		void Palette::SetCustomTitlebar() {
			WindowTitlebar = std::make_shared<Titlebar>(get_title(), true, true, true);

			WindowTitlebar->LinkWindow(this);

			set_titlebar(*WindowTitlebar);
		}

		void Palette::DisableShadows() {
			Extensions::ApplyCss(this, __STRINGIZE__(
				.window-frame {box-shadow: none;}
				.window-frame:backdrop {box-shadow: none;}
			));
		}

		bool Palette::on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state) {
			if (state == static_cast<Gdk::ModifierType>(0)) {
				switch (keyval) {
					case GDK_KEY_Escape:
						this->close();
						return true;

					default:
						return false;
				}
			}

			return false;

		}
	}
}