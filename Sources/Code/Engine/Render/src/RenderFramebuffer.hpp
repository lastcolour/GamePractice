#ifndef __RENDER_FRAMEBUFFER_HPP__
#define __RENDER_FRAMEBUFFER_HPP__

#include "Math/Vector.hpp"
#include "RenderTexture.hpp"

class RenderFramebuffer {
public:

    RenderFramebuffer();
    ~RenderFramebuffer();

    void bind();
    void unbind();

public:

    unsigned int framebufferId;
    RenderTexture texture;
};

#endif /* __RENDER_FRAMEBUFFER_HPP__ */