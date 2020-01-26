#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "Math/Matrix.hpp"

class RenderContext {
public:

    RenderContext();
    ~RenderContext();

    void setSrcMinusAlphaBlending(bool flag);

public:

    Mat4 proj2dMat;
    float dt;

private:

    bool isSrcMinusAlphaBlendingEnabled;
};

#endif /* __RENDER_CONTEXT_HPP__ */