#include <glmlv/Image2DRGBA.hpp>

#include <iostream>
#include <algorithm>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace glmlv
{

void Image2DRGBA::Deleter::operator ()(unsigned char * ptr) const
{
    stbi_image_free(ptr);
}

Image2DRGBA::Image2DRGBA(size_t width, size_t height):
    m_pData((unsigned char*) STBI_MALLOC(width * height * NumComponents * sizeof(unsigned char)))
{
}

Image2DRGBA::Image2DRGBA(size_t width, size_t height, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : Image2DRGBA(width, height)
{
    unsigned char * pPixel = m_pData.get();
    for (size_t i = 0, s = size(); i < size(); ++i)
    {
        pPixel[0] = r;
        pPixel[1] = g;
        pPixel[2] = b;
        pPixel[3] = a;
        pPixel += 4;
    }
}

void Image2DRGBA::flipY()
{
    unsigned char * pFirstLine = m_pData.get();
    unsigned char * pLastLine = m_pData.get() + (m_nHeight - 1) * m_nWidth * 4;

    while (pFirstLine < pLastLine)
    {
        for (size_t x = 0; x < m_nWidth * 4; ++x)
            std::swap(pFirstLine[x], pLastLine[x]);
        pFirstLine += m_nWidth * 4;
        pLastLine -= m_nWidth * 4;
    }
}

Image2DRGBA readImage(const fs::path& path)
{
    Image2DRGBA image;
    int w, h, n;
    image.m_pData.reset(stbi_load(path.string().c_str(), &w, &h, &n, Image2DRGBA::NumComponents));
    if (!image.m_pData)
    {
        std::cerr << "Unable to load image " << stbi_failure_reason() << std::endl;
        throw std::runtime_error(stbi_failure_reason());
    }

    image.m_nWidth = w;
    image.m_nHeight = h;

    return image;
}

void writeImage(const Image2DRGBA& image, const fs::path& path)
{
    const auto onFailure = []()
    {
        std::cerr << "Unable to write image" << std::endl;
        throw std::runtime_error("Unable to write image");
    };

    const auto ext = path.extension();
    if (ext == ".png")
    {
        if (stbi_write_png(path.string().c_str(), image.width(), image.height(), Image2DRGBA::NumComponents, image.data(), 0)) {
            onFailure();
        }
    }
    if (ext == ".bmp")
    {
        if (stbi_write_bmp(path.string().c_str(), image.width(), image.height(), Image2DRGBA::NumComponents, image.data())) {
            onFailure();
        }
    }
    if (ext == ".tga")
    {
        if (stbi_write_tga(path.string().c_str(), image.width(), image.height(), Image2DRGBA::NumComponents, image.data())) {
            onFailure();
        }
    }
}

}