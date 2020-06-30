#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "Math/Matrix.hpp"

enum class RenderBlendingType {
    NONE = 0,
    ONE_MINUS_SRC_MINUS_ALPHA
};

class RenderContext {
public:

    RenderContext();
    ~RenderContext();

    void setBlending(RenderBlendingType newBlendingType);

public:

    Mat4 proj2dMat;
    float dt;

private:

    RenderBlendingType blendingType;
};

#endif /* __RENDER_CONTEXT_HPP__ */