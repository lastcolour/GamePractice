#ifndef __RENDER_TEXTURE_FRAMEBUFFER_HPP__
#define __RENDER_TEXTURE_FRAMEBUFFER_HPP__

#include "Render/Color.hpp"
#include "Math/Vector.hpp"

#include <memory>

class RenderTextureFramebuffer {
public:

    RenderTextureFramebuffer();
    ~RenderTextureFramebuffer();

    bool bind();
    bool isBinded() const;
    bool isValid() const;
    void unbind();
    bool read();
    void clear();
    const void* getPtr() const;
    void reset();

    void setSize(const Vec2i& newSize);
    Vec2i getSize() const;
    ColorB getColor(int i, int j) const;

private:

    bool init();

private:

    Vec2i allocatedSize;
    Vec2i size;
    unsigned int framebufferId;
    unsigned int textureId;
    std::unique_ptr<Color::Color_RGBA_Byte[]> textureBuffer;
};

#endif /* __RENDER_TEXTURE_FRAMEBUFFER_HPP__ */