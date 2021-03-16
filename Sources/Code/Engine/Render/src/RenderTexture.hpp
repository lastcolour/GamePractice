#ifndef __RENDER_TEXTURE_HPP__
#define __RENDER_TEXTURE_HPP__

#include "Render/Color.hpp"
#include "Render/RenderCommon.hpp"

enum class TexLerpType {
    Linear = 0,
    Nearest
};

enum class TexWrapType {
    Repeat = 0,
    MirroredRepeat,
    ClamToEdge
};

class RenderTexture {
public:

    RenderTexture();
    ~RenderTexture();

    void writeR8(const Vec2i& pt, const Vec2i& subSize, const void* data);
    void writeRGB(const Vec2i& pt, const Vec2i& subSize, const void* data);
    void writeRGBA(const Vec2i& pt, const Vec2i& subSize, const void* data);

    void setPixelWrapType(TexWrapType sWrapType, TexWrapType tWrapType);
    void setPixelLerpType(TexLerpType minType, TexLerpType magType);

    void bind() const;
    void unbind() const;

    Vec2i getSize() const;
    bool resize(const Vec2i& newSize);
    bool resizeAndClear(const Vec2i& newSize);

public:

    ETextureType type;
    Vec2i size;
    unsigned int texId;
};

#endif /* __RENDER_TEXTURE_HPP__ */