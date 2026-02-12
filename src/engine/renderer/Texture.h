#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>
#include "Logger.h"

class Texture
{
    unsigned int ID;
    Logger logger;
public:
    Texture(std::string path);
    ~Texture();
    void bind(unsigned int unit = 0);
};
#endif