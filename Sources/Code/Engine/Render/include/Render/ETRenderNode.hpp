#ifndef __ET_RENDER_NODE_HPP__
#define __ET_RENDER_NODE_HPP__

#include "Math/AABB.hpp"
#include "Render/RenderCommon.hpp"

class StencilWirteReadData;

struct ETRenderNode {
    virtual ~ETRenderNode() = default;
    virtual bool ET_isVisible() const = 0;
    virtual void ET_hide() = 0;
    virtual void ET_show() = 0;
    virtual void ET_setZIndex(int newZIndex) = 0;
    virtual int ET_getZIndex() const = 0;
    virtual void ET_setAlphaMultiplier(float newAlphaMult) = 0;
    virtual float ET_getAlphaMultiplier() const = 0;
    virtual void ET_setNormalizationScale(float newNormScale) = 0;
    virtual float ET_getNormalizationScale() const = 0;
    virtual void ET_setStencilData(const StencilWirteReadData& newSteniclData) = 0;
    virtual bool ET_setEmitEvents(bool flag) = 0;
};

struct ETRenderNodeEvents {
    virtual ~ETRenderNodeEvents() = default;
    virtual void ET_onHidden(bool flag) = 0;
    virtual void ET_onZIndexChanged(int newZIndex) = 0;
    virtual void ET_onAlphaMultChanged(float newAlphaMult) = 0;
    virtual void ET_onNormScaleChanged(float newNormScale) = 0;
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
    virtual void ET_setFontType(EFontType newFontType) = 0;
    virtual EFontType ET_getFontType() const = 0;
    virtual AABB2D ET_getTextAABB() const = 0;
};

struct ETRenderImageLogic {
    virtual ~ETRenderImageLogic() = default;
    virtual TextureInfo ET_getTextureInfo() const = 0;
    virtual void ET_setTextureInfo(const TextureInfo& newTextureInfo) = 0;
};

enum ENinePatchSizeType {
    SmallestSidePct = 0,
    BiggestSidePct,
    Pixels
};

struct ETNinePatchImageLogic {
    virtual Vec2 ET_getPatchesTextureCoords() const = 0;
    virtual void ET_setPatchesTextureCoords(const Vec2& newPatchesTextureCoords) = 0;
    virtual void ET_setPatchSize(float NewSize) = 0;
    virtual float ET_getPatchSize() const = 0;
    virtual void ET_setPatchSizeType(ENinePatchSizeType newPatchSizeType) = 0;
    virtual ENinePatchSizeType ET_getPatchSizeType() const = 0;
    virtual Vec2 ET_getPatchesVertCoord() const = 0;
};

#endif /* __ET_RENDER_NODE_HPP__ */