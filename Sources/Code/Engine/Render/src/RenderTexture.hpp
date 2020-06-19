#ifndef __RENDER_TEXTURE_HPP__
#define __RENDER_TEXTURE_HPP__

#include "Math/Vector.hpp"
#include "Render/Color.hpp"

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

    void modifyRGBA(const Vec2i& startPt, const Vec2i& endPt, const ColorB* data);
    void setWrapType(TexWrapType sWrapType, TexWrapType tWrapType);
    void setTexLerpType(TexLerpType minType, TexLerpType magType);

public:

    unsigned int texId;
    Vec2i size;
};

#endif /* __RENDER_TEXTURE_HPP__ */