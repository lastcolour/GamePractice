#ifndef __RENDER_TEXTURE_FRAMEBUFFER_HPP__
#define __RENDER_TEXTURE_FRAMEBUFFER_HPP__

#include "Color.hpp"
#include "Math/Vector.hpp"

#include <memory>

class RenderTextureFramebuffer {
public:

    RenderTextureFramebuffer(int w, int h);
    ~RenderTextureFramebuffer();

    bool init();
    bool bind();
    bool isBinded() const;
    void unbind();
    bool read();
    void clear();
    const void* getPtr() const;

    Vec2i getSize() const;
    ColorB getColor(int i, int j) const;

private:

    Vec2i size;
    unsigned int framebufferId;
    unsigned int textureId;
    std::unique_ptr<Color::Color_RGBA_Byte[]> textureBuffer;
};

#endif /* __RENDER_TEXTURE_FRAMEBUFFER_HPP__ */