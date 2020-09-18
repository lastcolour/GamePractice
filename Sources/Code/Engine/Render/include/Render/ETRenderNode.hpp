#ifndef __ET_RENDER_NODE_HPP__
#define __ET_RENDER_NODE_HPP__

#include "Math/Vector.hpp"
#include "Math/AABB.hpp"
#include "Render/Color.hpp"
#include "Render/RenderCommon.hpp"

struct ETRenderNode {
    virtual ~ETRenderNode() = default;
    virtual bool ET_isVisible() const = 0;
    virtual void ET_hide() = 0;
    virtual void ET_show() = 0;
    virtual void ET_setDrawPriority(int newDrawPriority) = 0;
    virtual void ET_setAlphaMultiplier(float newAlphaMult) = 0;
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
    virtual void ET_setText(const char* text) = 0;
    virtual void ET_setColor(const ColorB& color) = 0;
    virtual void ET_setFontHeight(int fontHeight) = 0;
    virtual AABB2D ET_getTextAABB() const = 0;
};

struct ETRenderImageLogic {
    virtual ~ETRenderImageLogic() = default;
    virtual void ET_setImage(const char* imageName) = 0;
    virtual Vec2i ET_getImageSize() const = 0;
};

struct ETRenderColoredTexture {
    virtual ~ETRenderColoredTexture() = default;
    virtual void ET_setTextureColor(const ColorB& newColor) = 0;
};

#endif /* __ET_RENDER_NODE_HPP__ */