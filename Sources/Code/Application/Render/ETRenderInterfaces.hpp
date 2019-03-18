#ifndef __ET_RENDER_INTERFACES_HPP__
#define __ET_RENDER_INTERFACES_HPP__

#include "Math/Matrix.hpp"
#include "Render/Color.hpp"
#include "Core/ETPrimitives.hpp"

#include <memory>
#include <string>

class RenderGeometry;
class RenderMaterial;
class RenderTextureFramebuffer;

struct RenderContext {
    Mat4 proj2dMat;
};

struct ETRenderEvents {
public:

    virtual ~ETRenderEvents() = default;

    virtual void ET_onRender(const RenderContext& renderCtx) = 0;
    virtual void ET_onRenderPortResized() = 0;
};

struct ETRender {

    virtual ~ETRender() = default;

    virtual const ColorB& ET_getClearColor() const = 0;
    virtual void ET_setClearColor(const ColorB& col) = 0;
    virtual const Mat4& ET_getProj2DMat4() const = 0;
    virtual Vec2i ET_getRenderPort() const = 0;
    virtual void ET_setRenderToFramebuffer(RenderTextureFramebuffer* renderFb) = 0;
    virtual void ET_drawFrame() = 0;
    virtual std::shared_ptr<RenderGeometry> ET_createGeometry(const std::string& geomName) = 0;
    virtual std::shared_ptr<RenderMaterial> ET_createMaterial(const std::string& matName) = 0;
};

#endif /* __ET_RENDER_INTERFACES_HPP__ */