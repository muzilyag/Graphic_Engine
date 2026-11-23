#include "engine/renderer/Framebuffer.h"
#include "engine/platform/GLLoader.h"

Framebuffer::Framebuffer(int width, int height, FramebufferType type)
    : logger("Framebuffer"), type(type), width(width), height(height)
{
    create();
}

Framebuffer::~Framebuffer()
{
    destroy();
}

void Framebuffer::create()
{
    valid = false;
    if (width <= 0 || height <= 0)
    {
        return;
    }

    if (!glGenFramebuffers || !glBindFramebuffer || !glFramebufferTexture2D || !glCheckFramebufferStatus)
    {
        logger.error("Framebuffer extensions not available.");
        return;
    }

    if (glActiveTexture)
    {
        glActiveTexture(GL_TEXTURE0);
    }

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    if (type == FramebufferType::DEPTH_ONLY)
    {
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    else
    {
        int colorCount = (type == FramebufferType::HDR_MRT || type == FramebufferType::GBUFFER) ? 2 : 1;
        GLenum internalFormat = (type == FramebufferType::LDR_COLOR) ? GL_RGBA8 : GL_RGBA16F;
        GLenum pixelType = (type == FramebufferType::LDR_COLOR) ? GL_UNSIGNED_BYTE : GL_FLOAT;
        GLenum magFilter = (type == FramebufferType::GBUFFER) ? GL_NEAREST : GL_LINEAR;
        GLenum minFilter = magFilter;
        if (type == FramebufferType::HDR_COLOR_MIPMAP && glGenerateMipmap)
        {
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
        }

        glGenTextures(colorCount, colorTextures);

        GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        for (int i = 0; i < colorCount; ++i)
        {
            glBindTexture(GL_TEXTURE_2D, colorTextures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, pixelType, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            if (type == FramebufferType::HDR_COLOR_MIPMAP && glGenerateMipmap)
            {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, colorTextures[i], 0);
        }

        if (glDrawBuffers)
        {
            glDrawBuffers(colorCount, attachments);
        }

        if (type == FramebufferType::HDR_MRT || type == FramebufferType::GBUFFER)
        {
            if (glGenRenderbuffers && glBindRenderbuffer && glRenderbufferStorage && glFramebufferRenderbuffer)
            {
                glGenRenderbuffers(1, &depthRBO);
                glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
            }
        }
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status == GL_FRAMEBUFFER_COMPLETE)
    {
        valid = true;
    }
    else
    {
        logger.error("Framebuffer is not complete. Status: " + std::to_string(status));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::destroy()
{
    for (int i = 0; i < 2; ++i)
    {
        if (colorTextures[i] != 0)
        {
            glDeleteTextures(1, &colorTextures[i]);
            colorTextures[i] = 0;
        }
    }
    if (depthTexture != 0)
    {
        glDeleteTextures(1, &depthTexture);
        depthTexture = 0;
    }
    if (depthRBO != 0 && glDeleteRenderbuffers)
    {
        glDeleteRenderbuffers(1, &depthRBO);
        depthRBO = 0;
    }
    if (FBO != 0 && glDeleteFramebuffers)
    {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }
    valid = false;
}

void Framebuffer::resize(int newWidth, int newHeight)
{
    if (newWidth <= 0 || newHeight <= 0 || (newWidth == width && newHeight == height))
    {
        return;
    }

    destroy();
    width = newWidth;
    height = newHeight;
    create();
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void Framebuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindColorTexture(unsigned int attachmentIndex, unsigned int unit) const
{
    if (attachmentIndex >= 2 || colorTextures[attachmentIndex] == 0)
    {
        return;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, colorTextures[attachmentIndex]);
}

void Framebuffer::bindDepthTexture(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
}

void Framebuffer::blitColorTo(const Framebuffer& dst, unsigned int srcAttachment) const
{
    if (!valid || !dst.valid || !glBlitFramebuffer || srcAttachment >= 2)
    {
        return;
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + srcAttachment);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.FBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    GLenum filter = (width == dst.width && height == dst.height) ? GL_NEAREST : GL_LINEAR;
    glBlitFramebuffer(0, 0, width, height, 0, 0, dst.width, dst.height, GL_COLOR_BUFFER_BIT, filter);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::generateColorMipmaps(unsigned int attachmentIndex) const
{
    if (!valid || attachmentIndex >= 2 || colorTextures[attachmentIndex] == 0 || !glGenerateMipmap)
    {
        return;
    }

    if (glActiveTexture)
    {
        glActiveTexture(GL_TEXTURE0);
    }
    glBindTexture(GL_TEXTURE_2D, colorTextures[attachmentIndex]);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}