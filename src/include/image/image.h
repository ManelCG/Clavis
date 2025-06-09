#pragma once

#include <string>
#include <filesystem>

namespace Clavis {
	class Image {
	public:
		typedef struct Pixel {
			uint8_t R;
			uint8_t G;
			uint8_t B;
			uint8_t A;
		} Pixel;

		typedef struct PngImage {
			std::shared_ptr<uint8_t> Data;
			size_t Size;
			size_t Width;
			size_t Height;
			int Bpp;
		} PngImage;

		Image();
		Image(size_t w, size_t h);

		static Image* FromPath(std::filesystem::path path);

		Pixel Get(size_t x, size_t y);
		void Set(size_t x, size_t y, Pixel rgba);

		size_t GetWidth();
		size_t GetHeight();
		size_t GetSize();

		void Resize(size_t w, size_t h);

        // Inverts all color values leaving alpha unchanged
        // That is, all channels except alpha are mapped as x=>(255-x)
		void InvertColors();

		uint8_t* GetData();
		PngImage GetDataAsPng();


	private:
		std::shared_ptr<uint8_t> Data;

		size_t Width;
		size_t Height;
		size_t Size;
		const uint32_t Bpp = 4;

		void _Alloc(size_t w, size_t h);
		void _AllocPtr(std::shared_ptr<uint8_t>* ptr);

		size_t Index(size_t x, size_t y);
	};
}