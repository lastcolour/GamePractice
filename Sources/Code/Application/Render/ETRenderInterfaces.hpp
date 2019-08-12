#ifndef __ET_RENDER_INTERFACES_HPP__
#define __ET_RENDER_INTERFACES_HPP__

#include "Math/Matrix.hpp"
#include "Math/AABB.hpp"
#include "Render/Color.hpp"
#include "Core/ETPrimitives.hpp"

#include <memory>
#include <string>

class RenderGeometry;
class RenderMaterial;
class RenderTextureFramebuffer;
class RenderFont;
class RenderTexture;

struct ETRenderSimpleLogic {
    virtual ~ETRenderSimpleLogic() = default;
    virtual void ET_setMaterial(const char* matName) = 0;
    virtual void ET_setColor(const ColorB& color) = 0;
    virtual void ET_setSize(const Vec2& size) = 0;
};

struct ETRenderTextLogic {
    virtual ~ETRenderTextLogic() = default;
    virtual void ET_setMaterial(const char* matName) = 0;
    virtual void ET_setText(const char* text) = 0;
    virtual void ET_setColor(const ColorB& color) = 0;
    virtual void ET_setFontSize(int fontSize) = 0;
    virtual const AABB2D& ET_getTextAABB() const = 0;
};

struct ETRenderImageLogic {
    virtual ~ETRenderImageLogic() = default;
    virtual void ET_setImage(const char* imageName) = 0;
    virtual void ET_setMaterial(const char* matName) = 0;
    virtual Vec2i ET_getSize() const = 0;
};

struct RenderContext {
    Mat4 proj2dMat;
};

struct ETRenderEvents {
    virtual ~ETRenderEvents() = default;
    virtual void ET_onRender(const RenderContext& renderCtx) = 0;
    virtual void ET_onRenderPortResized() = 0;
};

struct ETRenderGeometryManager {
    virtual ~ETRenderGeometryManager() = default;
    virtual std::shared_ptr<RenderGeometry> ET_createGeometry(const char* geomName) = 0;
};

struct ETRenderMaterialManager {
    virtual ~ETRenderMaterialManager() = default;
    virtual std::shared_ptr<RenderMaterial> ET_createMaterial(const char* matName) = 0;
};

struct ETRenderFontManager {
    virtual ~ETRenderFontManager() = default;
    virtual std::shared_ptr<RenderFont> ET_createDefaultFont() = 0;
};

struct ETRenderTextureManger {
    virtual ~ETRenderTextureManger() = default;
    virtual std::shared_ptr<RenderTexture> ET_createTexture(const char* textureName) = 0;
};

struct ETRenderCamera {
    virtual ~ETRenderCamera() = default;
    virtual const Mat4& ET_getProj2DMat4() const = 0;
    virtual void ET_setRenderPort(const Vec2i& newViewPort) = 0;
    virtual const Vec2i& ET_getRenderPort() const = 0;
};

struct ETRender {
    virtual ~ETRender() = default;
    virtual const ColorB& ET_getClearColor() const = 0;
    virtual void ET_setClearColor(const ColorB& col) = 0;
    virtual void ET_setRenderToFramebuffer(RenderTextureFramebuffer* renderFb) = 0;
    virtual void ET_drawFrame() = 0;
};

struct ETRenderContextEvents {
    virtual ~ETRenderContextEvents();
    virtual void ET_onContextLost() = 0;
};

#endif /* __ET_RENDER_INTERFACES_HPP__ */