#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>
#include "engine/utils/Logger.h"

class Texture
{
    unsigned int ID = 0;
    Logger logger;
public:
    explicit Texture(const std::string& path);
    Texture(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    bool isValid() const { return ID != 0; }
    void bind(unsigned int unit = 0) const;
};
#endif