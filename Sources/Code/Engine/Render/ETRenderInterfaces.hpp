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
class RenderContext;

enum class PrimitiveGeometryType {
    Square = 0,
    Sqaure_Tex,
    Text_Vert_Chunk
};

struct ETRenderNode {
    virtual ~ETRenderNode() = default;
    virtual bool ET_isVisible() const = 0;
    virtual void ET_hide() = 0;
    virtual void ET_show() = 0;
    virtual void ET_setDrawPriority(int newDrawPriority) = 0;
    virtual void ET_setMaterial(const char* matName) = 0;
    virtual void ET_setGeometry(PrimitiveGeometryType geomType) = 0;
    virtual int ET_getDrawPriority() const = 0;
};

struct ETRenderRect {
    virtual ~ETRenderRect() = default;
    virtual void ET_setSize(const Vec2i& newSize) = 0;
    virtual Vec2i ET_getSize() const = 0;
};

struct ETRenderSimpleLogic {
    virtual ~ETRenderSimpleLogic() = default;
    virtual void ET_setColor(const ColorB& color) = 0;
};

struct ETRenderTextLogic {
    virtual ~ETRenderTextLogic() = default;
    virtual void ET_setFont(const char* fontName) = 0;
    virtual void ET_setText(const char* text) = 0;
    virtual void ET_setColor(const ColorB& color) = 0;
    virtual void ET_setFontSize(int fontSize) = 0;
    virtual AABB2D ET_getTextAABB() const = 0;
};

struct ETRenderImageLogic {
    virtual ~ETRenderImageLogic() = default;
    virtual void ET_setImage(const char* imageName) = 0;
    virtual void ET_setScale(const Vec2& newScale) = 0;
    virtual Vec2i ET_getOriginalSize() const = 0;
};

struct ETRenderColoredTexture {
    virtual ~ETRenderColoredTexture() = default;
    virtual void ET_setTextureColor(const ColorB& newColor) = 0;
};

struct ETRenderEvents {
    virtual ~ETRenderEvents() = default;
    virtual void ET_onRender(RenderContext& renderCtx) = 0;
    virtual void ET_onRenderPortResized() = 0;
};

struct ETRenderGeometryManager {
    virtual ~ETRenderGeometryManager() = default;
    virtual std::shared_ptr<RenderGeometry> ET_createGeometry(PrimitiveGeometryType geomType) = 0;
};

struct ETRenderMaterialManager {
    virtual ~ETRenderMaterialManager() = default;
    virtual std::shared_ptr<RenderMaterial> ET_createMaterial(const char* matName) = 0;
};

struct ETRenderFontManager {
    virtual ~ETRenderFontManager() = default;
    virtual std::shared_ptr<RenderFont> ET_createFont(const char* fontName) = 0;
    virtual std::shared_ptr<RenderFont> ET_createDefaultFont() = 0;
};

enum ETextureType {
    R8,
    RGB,
    RGBA
};

struct ETRenderTextureManger {
    virtual ~ETRenderTextureManger() = default;
    virtual std::shared_ptr<RenderTexture> ET_createTexture(const char* textureName, ETextureType texType) = 0;
    virtual std::shared_ptr<RenderTexture> ET_createEmptyTexture(const Vec2i& texSize, ETextureType texType) = 0;
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
    virtual void ET_drawFrame() = 0;
    virtual void ET_drawFrameToFramebufer(RenderTextureFramebuffer& renderFb) = 0;
    virtual void ET_updateRenderQueue() = 0;
};

struct ETRenderContextEvents {
    virtual ~ETRenderContextEvents() = default;
    virtual void ET_onContextSuspended() = 0;
    virtual void ET_onContextRestored() = 0;
    virtual void ET_onContextReCreated() = 0;
};

struct ETRenderResourceManager {
    virtual ~ETRenderResourceManager() = default;
    virtual void ET_forgetResoruces() = 0;
    virtual void ET_cleanUnused() = 0;
};

#endif /* __ET_RENDER_INTERFACES_HPP__ */