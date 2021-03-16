#ifndef __RENDER_FRAMEBUFFER_HPP__
#define __RENDER_FRAMEBUFFER_HPP__

#include "RenderTexture.hpp"

enum class EFramebufferType;

class RenderFramebuffer {
public:

    RenderFramebuffer();
    ~RenderFramebuffer();

    void bind();
    void unbind();

    void clear();
    bool resize(const Vec2i& newSize);

public:

    EFramebufferType type;
    RenderTexture color0;
    unsigned int framebufferId;
    unsigned int renderBufferId;
};

#endif /* __RENDER_FRAMEBUFFER_HPP__ */