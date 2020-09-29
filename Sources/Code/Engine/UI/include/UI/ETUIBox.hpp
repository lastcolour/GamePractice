#ifndef __ET_UI_BOX_HPP__
#define __ET_UI_BOX_HPP__

#include "Math/AABB.hpp"

#include <vector>

class UIBoxStyle;
class UIBoxMargin;
class UILayoutStyle;

struct ETUIElement {
    virtual ~ETUIElement() = default;
    virtual AABB2Di ET_getBox() const = 0;
    virtual UIBoxMargin ET_getMargin() const = 0;
    virtual void ET_setHostLayout(EntityId newLayoutId) = 0;
    virtual EntityId ET_getHostLayout() const = 0;
    virtual void ET_setZIndex(int newZIndex) = 0;
    virtual int ET_getZIndex() const = 0;
    virtual int ET_getZIndexDepth() const = 0;
    virtual void ET_setAlpha(float newAlpha) = 0;
    virtual float ET_getAlpha() const = 0;
    virtual void ET_show() = 0;
    virtual void ET_hide() = 0;
    virtual bool ET_isHidden() const = 0;
    virtual void ET_enable() = 0;
    virtual void ET_disable() = 0;
    virtual bool ET_isEnabled() const = 0;
    virtual void ET_setIgnoreTransform(bool flag) = 0;
};

struct ETUIElementEvents {
    virtual ~ETUIElementEvents() = default;
    virtual void ET_onBoxChanged(const AABB2Di& newAabb) = 0;
    virtual void ET_onZIndexChanged(int newZIndex) = 0;
    virtual void ET_onAlphaChanged(float newAlpha) = 0;
    virtual void ET_onHidden(bool flag) = 0;
    virtual void ET_onDisabled(bool flag) = 0;
    virtual void ET_onIngoreTransform(bool flag) = 0;
};

struct ETUIBox {
    virtual ~ETUIBox() = default;
    virtual const UIBoxStyle& ET_getStyle() const = 0;
    virtual void ET_setStyle(const UIBoxStyle& newStyle) = 0;
};

struct ETUILayout {
    virtual ~ETUILayout() = default;
    virtual const UILayoutStyle& ET_getStyle() const = 0;
    virtual void ET_setStyle(const UILayoutStyle& newStyle) = 0;
    virtual void ET_addItem(EntityId entityId) = 0;
    virtual std::vector<EntityId> ET_getItems() const = 0;
    virtual void ET_update() = 0;
    virtual const AABB2Di& ET_getCombinedBox() const = 0;
};

struct ETUILayoutEvents {
    virtual ~ETUILayoutEvents() = default;
    virtual void ET_onLayoutChanged(const AABB2Di& newCombinedBox) = 0;
};

struct ETUILabel {
    virtual ~ETUILabel() = default;
    virtual void ET_setText(const char* newText) = 0;
    virtual const char* ET_getText() const = 0;
    virtual float ET_getFontSize() const = 0;
    virtual void ET_setFontSize(float newSize) = 0;
    virtual void ET_setTextRender(EntityId newRenderId) = 0;
};

#endif /* __ET_UI_BOX_HPP__ */