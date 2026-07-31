#pragma once

#include <image/image.h>
#include <image/icons.h>

#include <gtkmm/picture.h>
#include <gdkmm/pixbuf.h>
#include <cstddef>

#include <filesystem>

namespace Clavis::GUI {
    class PictureInsert : public Gtk::Picture {
    public:
        PictureInsert();

        void Set(uint8_t* data, size_t w, size_t h);
        void Set(Image& im);

        // Takes ownership of the image. Required for images built at runtime, since the
        // underlying Gdk::Pixbuf borrows the pixel buffer rather than copying it.
        void Set(std::shared_ptr<Image> im);
        void Set(std::filesystem::path path);

        void SetIcon(Icons::IconDefinition name);

        void Resize(size_t w, size_t h);

        const std::shared_ptr<Image> GetUnderlyingImage();

    protected:

    private:
        Glib::RefPtr<Gdk::Pixbuf> Pixbuf;

        std::shared_ptr<Image> Im;
    };
}