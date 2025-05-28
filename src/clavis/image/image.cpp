#include <image/image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <image/stb_image_write.h>

#define STB_IMAGE_IMPLEMENTATION
#include <image/stb_image.h>

Clavis::Image::Image()
{
	Size = 0;
	Width = 0;
	Height = 0;
	Data = nullptr;
}

Clavis::Image* Clavis::Image::FromPath(std::filesystem::path path) {
	auto s = path.string();
	int x, y, channels;
	auto data = stbi_load(s.c_str(), &x, &y, &channels, 4);

	auto im = new Image(x, y);
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			int index = (i * x + j) * 4;
			Pixel p;
			p.R = data[index];
			p.G = data[index + 1];
			p.B = data[index + 2];
			p.A = data[index + 3];
			im->Set(j, i, p);
		}
	}

	return im;
}

void Clavis::Image::InvertColors() {
	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			auto p = Get(x, y);
			p.R = 255 - p.R;
			p.G = 255 - p.G;
			p.B = 255 - p.B;

			Set(x, y, p);
		}
	}
}

Clavis::Image::Image(size_t w, size_t h)
{
	_Alloc(w, h);
}

Clavis::Image::Pixel Clavis::Image::Get(size_t x, size_t y)
{
	return *((Pixel*)(&(Data.get()[Index(x, y)])));
}

void Clavis::Image::Set(size_t x, size_t y, Pixel rgba)
{
	auto p = (Pixel*)(Data.get() + Index(x, y));
	*p = rgba;
}

Clavis::Image::PngImage Clavis::Image::GetDataAsPng() {
	auto data = GetData();
	int outSize;
	auto retdata = stbi_write_png_to_mem(data, (int) Width*4, (int)Width, (int)Height, 4, &outSize);

	PngImage ret;
	ret.Data = std::shared_ptr<uint8_t>(retdata);
	ret.Size = outSize;
	ret.Width = Width;
	ret.Height = Height;
	ret.Bpp = Bpp;

	return ret;
}


size_t Clavis::Image::GetWidth()
{
	return Width;
}

size_t Clavis::Image::GetHeight()
{
	return Height;
}

size_t Clavis::Image::GetSize()
{
	return Size;
}

void Clavis::Image::Resize(size_t w, size_t h)
{
	Data = nullptr;
	_Alloc(w, h);
}

size_t Clavis::Image::Index(size_t x, size_t y)
{
	return ((y * Width) + x) * Bpp;
}

void Clavis::Image::_AllocPtr(std::shared_ptr<uint8_t>* ptr) {
	*ptr = std::shared_ptr<uint8_t>((uint8_t*) calloc(Size, sizeof(uint8_t)));
}

void Clavis::Image::_Alloc(size_t w, size_t h)
{
	Width = w;
	Height = h;
	Size = Width * Height * Bpp;

	_AllocPtr(&Data);
}

uint8_t* Clavis::Image::GetData() {
	return Data.get();
}