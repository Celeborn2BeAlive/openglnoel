#pragma once

#include <memory>
#include <glmlv/filesystem.hpp>

namespace glmlv
{

class Image2DRGBA
{
public:
    static const size_t NumComponents = 4;

    Image2DRGBA() = default;

    Image2DRGBA(size_t width, size_t height);

    Image2DRGBA(size_t width, size_t height, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    Image2DRGBA(const Image2DRGBA&) = delete;
    Image2DRGBA& operator =(const Image2DRGBA&) = delete;

    Image2DRGBA(Image2DRGBA&&) = default;
    Image2DRGBA& operator =(Image2DRGBA&&) = default;

    size_t width() const
    {
        return m_nWidth;
    }

    size_t height() const
    {
        return m_nHeight;
    }

    size_t size() const
    {
        return width() * height();
    }

    const unsigned char * data() const
    {
        return m_pData.get();
    }

    unsigned char * data()
    {
        return m_pData.get();
    }

    const unsigned char * operator ()(size_t x, size_t y) const
    {
        return m_pData.get() + (x + y * m_nWidth) * NumComponents;
    }

    unsigned char * operator ()(size_t x, size_t y)
    {
        return const_cast<unsigned char*>((*this)(x, y));
    }

    void flipY(); // Flip the image along its y axis

private:
    friend Image2DRGBA readImage(const fs::path& path);

    struct Deleter
    {
        void operator ()(unsigned char *) const;
    };

    std::unique_ptr<unsigned char[], Deleter> m_pData;
    size_t m_nWidth = 0;
    size_t m_nHeight = 0;
};

// Supported formats for reading are:
////    JPEG baseline & progressive(12 bpc / arithmetic not supported, same as stock IJG lib)
////    PNG 1 / 2 / 4 / 8 - bit - per - channel(16 bpc not supported)
////    
////    TGA(not sure what subset, if a subset)
////    BMP non - 1bpp, non - RLE
////    PSD(composited view only, no extra channels, 8 / 16 bit - per - channel)
////    
////    GIF(*comp always reports as 4 - channel)
////    HDR(radiance rgbE format)
////    PIC(Softimage PIC)
////    PNM(PPM and PGM binary only)
Image2DRGBA readImage(const fs::path& path);

// Supported formats for writing are png, bmp and tga
void writeImage(const Image2DRGBA& image, const fs::path& path);

}