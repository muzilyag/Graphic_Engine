#ifndef IMAGELOADER_H
#define IMAGELOADER_H
#include <string>
#include <vector>

struct ImageData
{
    int width = 0;
    int height = 0;
    int channels = 0;
    std::vector<unsigned char> pixels;

    bool isValid() const
    {
        return width > 0 && height > 0 && (channels == 3 || channels == 4) && !pixels.empty();
    }
};

class ImageLoader
{
public:
    static ImageData load(const std::string& path);
    static ImageData loadBMP(const std::string& path);
    static ImageData loadTGA(const std::string& path);
};
#endif