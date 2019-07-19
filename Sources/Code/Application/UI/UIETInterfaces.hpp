#ifndef __UI_ET_INTERFACES_HPP__
#define __UI_ET_INTERFACES_HPP__

#include "Math/AABB.hpp"
#include "Core/ETPrimitives.hpp"

struct UIStyle;

enum class UIListType {
    Vertical = 0,
    Horizontal
};

struct ETUIBox {
    virtual ~ETUIBox() = default;
    virtual const AABB2Di& ET_getAabb2di() const = 0;
    virtual void ET_setCenter(const Vec2i& pos) = 0;
    virtual void ET_alignInBox(const AABB2Di& alignBox) = 0;
    virtual void ET_boxResize() = 0;
    virtual const UIStyle& ET_getStyle() const = 0;
    virtual void ET_setStyle(const UIStyle& newStyle) = 0;
    virtual void ET_addChildElement(EntityId childId) = 0;
    virtual void ET_boxResizeInsize(const AABB2Di& resizeBox) = 0;
};

struct ETUIButton {
    virtual ~ETUIButton() = default;
    virtual void ET_onPress() = 0;
};

struct ETUILabel {
    virtual ~ETUILabel() = default;
    virtual void ET_setText(const std::string& text) = 0;
};

struct ETUIList {
    virtual ~ETUIList() = default;
    virtual void ET_setType(UIListType listType) = 0;
};

struct ETUIEventManager {
    virtual ~ETUIEventManager() = default;
    virtual void ET_onEvent(const std::string& eventName) = 0;
};

#endif /* __UI_ET_INTERFACES_HPP__ */