#include <GUI/palettes/Palette.h>

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
	}
}