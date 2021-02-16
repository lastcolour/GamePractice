#ifndef __ET_UI_SCROLL_AREA_HPP__
#define __ET_UI_SCROLL_AREA_HPP__

#include "Core/Core.hpp"
#include "Math/AABB.hpp"

class UIScrollAreaStyle;

struct ETUIScrollArea {
    virtual ~ETUIScrollArea() = default;
    virtual void ET_setTarget(EntityId newTargetId) = 0;
    virtual EntityId ET_getTarget() const = 0;
    virtual void ET_setStyle(const UIScrollAreaStyle& newStyle) = 0;
    virtual const UIScrollAreaStyle& ET_getStyle() const = 0;
    virtual void ET_enableKinematicScroll(bool flag) = 0;
    virtual float ET_getScrollProgress() const = 0;
    virtual void ET_setScrollProgress(float newScrollProgress) = 0;
    virtual void ET_setTargetPosClamped(const Vec2i& newScrollPt) = 0;
    virtual AABB2Di ET_getScrollArea() const = 0;
};

struct ETUIScrollFocus {
    virtual ~ETUIScrollFocus() = default;
    virtual void ET_setFocusToEntity(EntityId focusEntId) = 0;
};

#endif /* __ET_UI_SCROLL_AREA_HPP__ */