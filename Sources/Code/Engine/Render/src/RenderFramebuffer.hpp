#ifndef __RENDER_FRAMEBUFFER_HPP__
#define __RENDER_FRAMEBUFFER_HPP__

#include "RenderTexture.hpp"
#include "Render/RenderCommon.hpp"

class RenderFramebuffer {
public:

    RenderFramebuffer();
    ~RenderFramebuffer();

    void bind();
    void unbind();
    bool isBound() const;

    void clear();
    bool resize(const Vec2i& newSize);
    Vec2i getSize() const;

private:

    RenderFramebuffer(const RenderFramebuffer&) = delete;
    RenderFramebuffer& operator=(const RenderFramebuffer&) = delete;

public:

    EFramebufferType type;
    RenderTexture color0;
    unsigned int framebufferId;
    unsigned int renderBufferId;
    bool bound;
};

#endif /* __RENDER_FRAMEBUFFER_HPP__ */