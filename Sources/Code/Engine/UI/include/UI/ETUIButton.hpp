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

struct ETUIButton {
    virtual ~ETUIButton() = default;
    virtual void ET_setEventName(const char* newEventName) = 0;
    virtual float ET_getPressDuration() const = 0;
};

struct ETUIButtonEventManager {
    virtual ~ETUIButtonEventManager() = default;
    virtual bool ET_isSomeButtonPressed() const = 0;
    virtual void ET_setButtonPressed(bool flag) = 0;
};

struct ETUIButtonPressAnimationEvents {
    virtual ~ETUIButtonPressAnimationEvents() = default;
    virtual void ET_onPressAnimationEnd() = 0;
};

struct ETUIButtonPressAnimation {
    virtual ~ETUIButtonPressAnimation() = default;
    virtual void ET_startPressAnimation() = 0;
    virtual float ET_getAnimationDuration() const = 0;
};

#endif /* __ET_UI_BUTTON_HPP__ */