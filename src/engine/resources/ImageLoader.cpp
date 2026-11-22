#include "engine/resources/ImageLoader.h"
#include "engine/utils/Logger.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>

namespace
{
    std::string getExtensionLower(const std::string& path)
    {
        size_t dot = path.find_last_of('.');
        if (dot == std::string::npos)
        {
            return "";
        }
        std::string ext = path.substr(dot);
        std::transform(ext.begin(), ext.end(), ext.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return ext;
    }

    void flipVertically(std::vector<unsigned char>& pixels, int width, int height, int channels)
    {
        size_t rowBytes = static_cast<size_t>(width) * static_cast<size_t>(channels);
        std::vector<unsigned char> temp(rowBytes);
        for (int y = 0; y < height / 2; ++y)
        {
            unsigned char* top = pixels.data() + static_cast<size_t>(y) * rowBytes;
            unsigned char* bottom = pixels.data() + static_cast<size_t>(height - 1 - y) * rowBytes;
            std::memcpy(temp.data(), top, rowBytes);
            std::memcpy(top, bottom, rowBytes);
            std::memcpy(bottom, temp.data(), rowBytes);
        }
    }

    void convertBGRToRGB(unsigned char* data, size_t pixelCount, int channels)
    {
        for (size_t i = 0; i < pixelCount; ++i)
        {
            size_t base = i * static_cast<size_t>(channels);
            std::swap(data[base + 0], data[base + 2]);
        }
    }
}

ImageData ImageLoader::load(const std::string& path)
{
    std::string ext = getExtensionLower(path);
    if (ext == ".bmp")
    {
        return loadBMP(path);
    }
    if (ext == ".tga")
    {
        return loadTGA(path);
    }

    Logger logger("ImageLoader");
    logger.error("Unsupported image extension: " + path);
    return {};
}

ImageData ImageLoader::loadBMP(const std::string& path)
{
    Logger logger("ImageLoader");
    ImageData img;

    std::ifstream f(path, std::ios::binary);
    if (!f)
    {
        logger.error("File not found: " + path);
        return {};
    }

    unsigned char head[54];
    f.read(reinterpret_cast<char*>(head), sizeof(head));
    if (!f || head[0] != 'B' || head[1] != 'M')
    {
        logger.error("Invalid BMP header: " + path);
        return {};
    }

    uint32_t offset = 0;
    int32_t w = 0;
    int32_t rawH = 0;
    uint16_t bpp = 0;
    uint32_t compression = 0;

    std::memcpy(&offset,      &head[10], sizeof(offset));
    std::memcpy(&w,           &head[18], sizeof(w));
    std::memcpy(&rawH,        &head[22], sizeof(rawH));
    std::memcpy(&bpp,         &head[28], sizeof(bpp));
    std::memcpy(&compression, &head[30], sizeof(compression));

    if (w <= 0 || rawH == 0 || (bpp != 24 && bpp != 32) || compression != 0)
    {
        logger.error("Unsupported BMP format: " + path);
        return {};
    }

    bool topDown = (rawH < 0);
    int h = std::abs(rawH);
    int channels = bpp / 8;

    img.width = w;
    img.height = h;
    img.channels = channels;
    img.pixels.resize(static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(channels));

    f.seekg(offset, std::ios::beg);

    size_t rowDataBytes = static_cast<size_t>(w) * static_cast<size_t>(channels);
    size_t rowStride = (rowDataBytes + 3) & ~static_cast<size_t>(3);
    std::streamoff padding = static_cast<std::streamoff>(rowStride - rowDataBytes);

    for (int i = 0; i < h; ++i)
    {
        int dstRow = topDown ? (h - 1 - i) : i;
        unsigned char* rowPtr = img.pixels.data() + static_cast<size_t>(dstRow) * rowDataBytes;
        f.read(reinterpret_cast<char*>(rowPtr), static_cast<std::streamsize>(rowDataBytes));
        if (!f)
        {
            logger.error("Unexpected EOF while reading BMP pixels: " + path);
            return {};
        }
        if (padding > 0)
        {
            f.seekg(padding, std::ios::cur);
        }
    }

    convertBGRToRGB(img.pixels.data(), static_cast<size_t>(w) * static_cast<size_t>(h), channels);
    return img;
}

ImageData ImageLoader::loadTGA(const std::string& path)
{
    Logger logger("ImageLoader");
    ImageData img;

    std::ifstream f(path, std::ios::binary);
    if (!f)
    {
        logger.error("File not found: " + path);
        return {};
    }

    unsigned char head[18];
    f.read(reinterpret_cast<char*>(head), sizeof(head));
    if (!f)
    {
        logger.error("Invalid TGA header: " + path);
        return {};
    }

    uint8_t idLength = head[0];
    uint8_t colorMapType = head[1];
    uint8_t imageType = head[2];
    uint16_t width = 0;
    uint16_t height = 0;
    uint8_t bpp = head[16];
    uint8_t descriptor = head[17];

    std::memcpy(&width,  &head[12], sizeof(width));
    std::memcpy(&height, &head[14], sizeof(height));

    if (colorMapType != 0 || (imageType != 2 && imageType != 10) || width == 0 || height == 0 || (bpp != 24 && bpp != 32))
    {
        logger.error("Unsupported TGA format: " + path);
        return {};
    }

    if (idLength > 0)
    {
        f.seekg(idLength, std::ios::cur);
    }

    int channels = bpp / 8;
    size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
    size_t byteCount = pixelCount * static_cast<size_t>(channels);

    img.width = width;
    img.height = height;
    img.channels = channels;
    img.pixels.resize(byteCount);

    if (imageType == 2)
    {
        f.read(reinterpret_cast<char*>(img.pixels.data()), static_cast<std::streamsize>(byteCount));
        if (!f)
        {
            logger.error("Unexpected EOF while reading TGA pixels: " + path);
            return {};
        }
        convertBGRToRGB(img.pixels.data(), pixelCount, channels);
    }
    else
    {
        size_t currentPixel = 0;
        unsigned char pixelBuf[4];

        while (currentPixel < pixelCount)
        {
            unsigned char packetHeader = 0;
            f.read(reinterpret_cast<char*>(&packetHeader), 1);
            if (!f)
            {
                logger.error("Unexpected EOF in RLE TGA packet: " + path);
                return {};
            }

            size_t count = static_cast<size_t>(packetHeader & 0x7F) + 1;
            if ((packetHeader & 0x80) != 0)
            {
                f.read(reinterpret_cast<char*>(pixelBuf), channels);
                if (!f)
                {
                    logger.error("Unexpected EOF in RLE TGA color: " + path);
                    return {};
                }
                std::swap(pixelBuf[0], pixelBuf[2]);

                for (size_t i = 0; i < count && currentPixel < pixelCount; ++i)
                {
                    std::memcpy(img.pixels.data() + currentPixel * static_cast<size_t>(channels), pixelBuf, channels);
                    ++currentPixel;
                }
            }
            else
            {
                for (size_t i = 0; i < count && currentPixel < pixelCount; ++i)
                {
                    f.read(reinterpret_cast<char*>(pixelBuf), channels);
                    if (!f)
                    {
                        logger.error("Unexpected EOF in raw TGA packet: " + path);
                        return {};
                    }
                    std::swap(pixelBuf[0], pixelBuf[2]);
                    std::memcpy(img.pixels.data() + currentPixel * static_cast<size_t>(channels), pixelBuf, channels);
                    ++currentPixel;
                }
            }
        }
    }

    bool topDown = (descriptor & 0x20) != 0;
    if (topDown)
    {
        flipVertically(img.pixels, img.width, img.height, img.channels);
    }

    return img;
}