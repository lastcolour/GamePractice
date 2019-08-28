#ifndef __RENDER_TEXTURE_HPP__
#define __RENDER_TEXTURE_HPP__

#include "Math/Vector.hpp"

class RenderTexture {
public:

    RenderTexture();
    ~RenderTexture();

    void bind();
    void undind();

public:

    unsigned int texId;
    Vec2i size;
};

#endif /* __RENDER_TEXTURE_HPP__ */