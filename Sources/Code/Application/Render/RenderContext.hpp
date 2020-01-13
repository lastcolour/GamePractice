#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "Math/Matrix.hpp"

class RenderContext {
public:

    RenderContext();
    ~RenderContext();

    void setSrcMinusAlphaBlending(bool flag) const;

public:

    Mat4 proj2dMat;

private:

    mutable bool isSrcMinusAlphaBlendingEnabled;
};

#endif /* __RENDER_CONTEXT_HPP__ */