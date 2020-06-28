#ifndef __ET_UI_BUTTON_HPP__
#define __ET_UI_BUTTON_HPP__

#include "Math/AABB.hpp"

struct ETUIInteractionBox {
    virtual ~ETUIInteractionBox() = default;
    virtual void ET_onPress() = 0;
    virtual void ET_onHover(bool flag) = 0;
    virtual bool ET_isHovered() const = 0;
    virtual AABB2Di ET_getHitBox() const = 0;
};

struct ETUIButtonEventManager {
    virtual ~ETUIButtonEventManager() = default;
    virtual EntityId ET_getActiveButton() const = 0;
    virtual void ET_setActiveButton(EntityId buttonId) = 0;
};

#endif /* __ET_UI_BUTTON_HPP__ */