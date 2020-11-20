#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "Math/Matrix.hpp"
#include "Render/Color.hpp"
#include "Render/RenderCommon.hpp"

#include <vector>

class RenderFramebuffer;

enum class RenderBlendingType {
    NONE = 0,
    ONE_MINUS_SRC_MINUS_ALPHA
};

class RenderContext {
public:

    RenderContext();
    ~RenderContext();

    void setStencilState(const StencilWirteReadData& newStencilState);
    void setBlending(RenderBlendingType newBlendingType);
    RenderBlendingType getBlendingType() const;

public:

    Mat4 proj2dMat;
    float dt;
    RenderFramebuffer* mainFBO;
    std::vector<RenderFramebuffer*> exraFBOs;

private:

    StencilWirteReadData stencilState;
    RenderBlendingType blendingType;
};

#endif /* __RENDER_CONTEXT_HPP__ */