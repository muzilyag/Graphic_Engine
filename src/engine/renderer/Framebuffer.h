#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "engine/utils/Logger.h"

enum class FramebufferType
{
    DEPTH_ONLY,
    LDR_COLOR,
    HDR_COLOR,
    HDR_COLOR_MIPMAP,
    HDR_MRT,
    GBUFFER
};

class Framebuffer
{
    Logger logger;
    FramebufferType type = FramebufferType::DEPTH_ONLY;
    unsigned int FBO = 0;
    unsigned int colorTextures[2] = { 0, 0 };
    unsigned int depthTexture = 0;
    unsigned int depthRBO = 0;
    int width = 0;
    int height = 0;
    bool valid = false;

    void create();
    void destroy();
public:
    Framebuffer(int width, int height, FramebufferType type = FramebufferType::DEPTH_ONLY);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    void resize(int newWidth, int newHeight);
    void bind() const;
    void unbind() const;
    void bindColorTexture(unsigned int attachmentIndex, unsigned int unit) const;
    void bindDepthTexture(unsigned int unit) const;
    void blitColorTo(const Framebuffer& dst, unsigned int srcAttachment = 0) const;
    void generateColorMipmaps(unsigned int attachmentIndex = 0) const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isValid() const { return valid; }
};
#endif