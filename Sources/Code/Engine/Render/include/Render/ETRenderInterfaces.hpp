#ifndef __ET_RENDER_INTERFACES_HPP__
#define __ET_RENDER_INTERFACES_HPP__

#include "Math/Matrix.hpp"
#include "Math/AABB.hpp"
#include "Render/Color.hpp"
#include "Core/ETPrimitives.hpp"

#include <memory>
#include <string>

class RenderNode;
class RenderContext;
class RenderTextureFramebuffer;

struct ETRender {
    virtual ~ETRender() = default;
    virtual const ColorB& ET_getClearColor() const = 0;
    virtual void ET_setClearColor(const ColorB& col) = 0;
    virtual void ET_drawFrame() = 0;
    virtual void ET_drawFrameToFramebufer(RenderTextureFramebuffer& renderFb) = 0;
    virtual void ET_registerNode(RenderNode* renderNode) = 0;
};

struct ETRenderContextEvents {
    virtual ~ETRenderContextEvents() = default;
    virtual void ET_onContextSuspended() = 0;
    virtual void ET_onContextRestored() = 0;
    virtual void ET_onContextReCreated() = 0;
};

#endif /* __ET_RENDER_INTERFACES_HPP__ */