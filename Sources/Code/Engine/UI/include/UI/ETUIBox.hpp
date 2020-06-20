#ifndef __ET_UI_BOX_HPP__
#define __ET_UI_BOX_HPP__

#include "Math/AABB.hpp"

class UIBoxStyle;
class UIBoxMargin;
class UILayoutStyle;

struct ETUIBoxEvents {
    virtual ~ETUIBoxEvents() = default;
    virtual void ET_onBoxResized() = 0;
};

struct ETUIBox {
    virtual ~ETUIBox() = default;
    virtual const AABB2Di& ET_getBox() const = 0;
    virtual const UIBoxStyle& ET_getStyle() const = 0;
    virtual void ET_setStyle(const UIBoxStyle& newStyle) = 0;
    virtual const UIBoxMargin& ET_getMargin() const = 0;
    virtual void ET_setLayout(EntityId newLayoutId) = 0;
    virtual EntityId ET_getLayout() = 0;
};

struct ETUILayout {
    virtual ~ETUILayout() = default;
    virtual const UILayoutStyle& ET_getStyle() const = 0;
    virtual void ET_setStyle(const UILayoutStyle& newStyle) = 0;
    virtual void ET_addItem(EntityId entityId) = 0;
    virtual void ET_update() = 0;
};

struct ETUIVisibleElement {
    virtual ~ETUIVisibleElement() = default;
    virtual void ET_show() = 0;
    virtual void ET_hide() = 0;
    virtual bool ET_isVisible() const = 0;
    virtual int ET_getZIndex() const = 0;
    virtual void ET_setZIndex(int newZIndex) = 0;
};

struct ETUILabel {
    virtual ~ETUILabel() = default;
    virtual void ET_setText(const char* text) = 0;
    virtual const char* ET_getText() const = 0;
};

struct ETUIImage {
    virtual ~ETUIImage() = default;
    virtual void ET_setImage(const char* image) = 0; 
};

struct ETUISwitchControl {
    virtual ~ETUISwitchControl() = default;
    virtual bool ET_isEnabled() const = 0;
};


#endif /* __ET_UI_BOX_HPP__ */