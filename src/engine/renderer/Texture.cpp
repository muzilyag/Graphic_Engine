#include "Texture.h"
#include "GLLoader.h"
#include <fstream>

Texture::Texture(std::string path) : logger("Texture")
{
    std::ifstream f(path, std::ios::binary);
    if (!f) 
    {
        logger.error("Not founded file: " + std::string(path));
        return;
    }
    unsigned char head[54];
    f.read((char*)head, 54);
    int w = *(int*)&head[18];
    int h = *(int*)&head[22];
    int offset = *(int*)&head[10];

    unsigned char* data = new unsigned char[w * h * 3];
    f.seekg(offset, std::ios::beg);
    int row = (w * 3 + 3) & ~3;
    for (int i = 0; i < h; i++) 
    {
        f.read((char*)(data + (i * w * 3)), w * 3);
        f.seekg(row - w * 3, std::ios::cur);
    }

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    logger.info("Texture loaded from file: " + path);
    delete[] data;
}

Texture::~Texture() 
{
    glDeleteTextures(1, &ID);
}

void Texture::bind(unsigned int unit) 
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}