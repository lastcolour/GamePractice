#ifndef __ET_UI_LAYOUT_HPP__
#define __ET_UI_LAYOUT_HPP__

#include "Math/AABB.hpp"

class UILayoutStyle;

struct ETUIElemAligner {
    virtual ~ETUIElemAligner() = default;
    virtual void ET_reAlign() = 0;
};

struct ETUILayout {
    virtual ~ETUILayout() = default;
    virtual const UILayoutStyle& ET_getStyle() const = 0;
    virtual void ET_setStyle(const UILayoutStyle& newStyle) = 0;
    virtual void ET_addItem(EntityId entityId) = 0;
    virtual std::vector<EntityId> ET_getItems() const = 0;
    virtual const AABB2D& ET_getCombinedBox() const = 0;
};

struct ETUILayoutEvents {
    virtual ~ETUILayoutEvents() = default;
    virtual void ET_onLayoutChanged(const AABB2D& newCombinedBox) = 0;
};

#endif /* __ET_UI_LAYOUT_HPP__ */