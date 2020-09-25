#ifndef __ET_UI_SCROLL_AREA_HPP__
#define __ET_UI_SCROLL_AREA_HPP__

#include "Core/Core.hpp"

class UIScrollAreaStyle;

struct ETUIScrollArea {
    virtual ~ETUIScrollArea() = default;
    virtual void ET_setTarget(EntityId newTargetId) = 0;
    virtual EntityId ET_getTarget() const = 0;
    virtual void ET_setStyle(const UIScrollAreaStyle& newStyle) = 0;
    virtual const UIScrollAreaStyle& ET_getStyle() const = 0;
};

#endif /* __ET_UI_SCROLL_AREA_HPP__ */