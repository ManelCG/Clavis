#include <GUI/components/PictureInsert.h>

#include <settings/Settings.h>

#include <image/stb_image.h>


namespace Clavis {
	namespace GUI {
		PictureInsert::PictureInsert() {

		}

		void PictureInsert::Set(Image& im) {
			auto data = im.GetData();
			auto w = im.GetWidth();
			auto h = im.GetHeight();

			Set(data, w, h);
		}

		void PictureInsert::Set(uint8_t* data, size_t w, size_t h) {
			Pixbuf = Gdk::Pixbuf::create_from_data(data, Gdk::Colorspace::RGB, true, 8, (int)w, (int)h, (int)w * 4);
			set_pixbuf(Pixbuf);
		}

		void PictureInsert::Set(std::filesystem::path path) {
			Pixbuf = Gdk::Pixbuf::create_from_file(path.string());
			set_pixbuf(Pixbuf);
		}

		void PictureInsert::SetIcon(Icons::IconDefinition def) {
			auto name = std::string(def.Name);
			std::string path = "assets/icons/" + name;

			Im = std::shared_ptr<Image>(Image::FromPath(path));

			if (Settings::DO_USE_DARK_THEME.GetValue() && def.Inversible)
				Im->InvertColors();

			Set(*Im);

			set_valign(Gtk::Align::CENTER);
			set_halign(Gtk::Align::CENTER);
			set_hexpand(false);
			set_vexpand(false);
			set_can_shrink(false);
		}

		void PictureInsert::Resize(const size_t w, const size_t h) {
			Pixbuf = Pixbuf->scale_simple(w, h, Gdk::InterpType::BILINEAR);
			set_pixbuf(Pixbuf);
		}


        const std::shared_ptr<Image> PictureInsert::GetUnderlyingImage() {
            return Im;
        }

        //void PictureInsert::SetIcon(std::string name) {
		//	std::string path = "assets/icons/";

		//	if (Settings::GetAsValue<bool>(DO_USE_DARK_THEME))
		//		path += "dark/";
		//	else
		//		path += "light/";

		//	path += name;

		//	Set(path);
		//}
	}
}