#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "Math/Matrix.hpp"
#include "Render/Color.hpp"
#include "Render/RenderCommon.hpp"

#include <vector>

class RenderFramebuffer;

struct BlendMode {
    BlendType src;
    BlendType dst;
};
class RenderContext {
public:

    RenderContext();
    ~RenderContext();

    void setStencilState(const StencilWirteReadData& newStencilState);
    void setBlending(const BlendMode& newBlendMode);

public:

    Mat4 proj2dMat;
    float dt;
    RenderFramebuffer* mainFBO;
    std::vector<RenderFramebuffer*> exraFBOs;

private:

    StencilWirteReadData stencilState;
    BlendMode blendMode;
};

#endif /* __RENDER_CONTEXT_HPP__ */