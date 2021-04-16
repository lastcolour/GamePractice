#ifndef __RENDER_TEXTURE_HPP__
#define __RENDER_TEXTURE_HPP__

#include "Render/TextureInfo.hpp"

class RenderTexture {
public:

    RenderTexture();
    ~RenderTexture();

    void writeR8(const Vec2i& pt, const Vec2i& subSize, const void* data);
    void writeRGB(const Vec2i& pt, const Vec2i& subSize, const void* data);
    void writeRGBA(const Vec2i& pt, const Vec2i& subSize, const void* data);

    void setWrapType(ETextureWrapType stWrapType, ETextureWrapType tWrapType);
    void setLerpType(ETextureLerpType minType, ETextureLerpType magType);

    void bind() const;
    void unbind() const;

    bool resize(const Vec2i& newSize);
    bool clear();

    const Vec2i& getSize() const;

public:

    Vec2i size;
    unsigned int texId;
    ETextureLerpType minLerpType;
    ETextureLerpType magLerpType;
    ETextureWrapType sWrapType;
    ETextureWrapType tWrapType;
    ETextureDataType dataType;
};

#endif /* __RENDER_TEXTURE_HPP__ */