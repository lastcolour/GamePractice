#ifndef __ET_RENDER_NODE_HPP__
#define __ET_RENDER_NODE_HPP__

#include "Math/Vector.hpp"
#include "Math/AABB.hpp"
#include "Render/Color.hpp"

class StencilWirteReadData;

struct ETRenderNode {
    virtual ~ETRenderNode() = default;
    virtual bool ET_isVisible() const = 0;
    virtual void ET_hide() = 0;
    virtual void ET_show() = 0;
    virtual void ET_setDrawPriority(int newDrawPriority) = 0;
    virtual void ET_setAlphaMultiplier(float newAlphaMult) = 0;
    virtual void ET_setNormalizationScale(float newNormScale) = 0;
    virtual float ET_getNormalizationScale() const = 0;
    virtual int ET_getDrawPriority() const = 0;
    virtual void ET_setStencilData(const StencilWirteReadData& newSteniclData) = 0;
};

struct ETRenderRect {
    virtual ~ETRenderRect() = default;
    virtual void ET_setSize(const Vec2& newSize) = 0;
    virtual Vec2 ET_getSize() const = 0;
};

struct ETRenderSimpleLogic {
    virtual ~ETRenderSimpleLogic() = default;
    virtual void ET_setColor(const ColorB& color) = 0;
};

struct ETRenderTextLogic {
    virtual ~ETRenderTextLogic() = default;
    virtual void ET_setText(const char* text) = 0;
    virtual void ET_setColor(const ColorB& color) = 0;
    virtual void ET_setFontHeight(float newFontHeight) = 0;
    virtual AABB2D ET_getTextAABB() const = 0;
};

struct ETRenderImageLogic {
    virtual ~ETRenderImageLogic() = default;
    virtual void ET_setImage(const char* imageName) = 0;
    virtual void ET_setTintColor(const ColorB& newTintColor) = 0;
};

#endif /* __ET_RENDER_NODE_HPP__ */