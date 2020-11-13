#ifndef __ET_UI_ANIMATION_HPP__
#define __ET_UI_ANIMATION_HPP__

#include "Core/Core.hpp"

struct ETUIPressAnimation {
    virtual ~ETUIPressAnimation() = default;
    virtual void ET_playPress(EntityId triggerId) = 0;
};

struct ETUIPressAnimationEvents {
    virtual ~ETUIPressAnimationEvents() = default;
    virtual void ET_onPressPlayed() = 0;
};

struct ETUIHighlightAnimation {
    virtual ~ETUIHighlightAnimation() = default;
    virtual void ET_playHightlight(EntityId triggerId) = 0;
};

struct ETUIHighlightAnimationEvents {
    virtual ~ETUIHighlightAnimationEvents() = default;
    virtual void ET_onHighlightPlayed() = 0;
};

struct ETUIViewAppearAnimation {
    virtual ~ETUIViewAppearAnimation() = default;
    virtual void ET_playAppear(EntityId triggerId) = 0;
    virtual void ET_playDissapear(EntityId triggerId) = 0;
};

struct ETUIViewAppearAnimationEvents {
    virtual ~ETUIViewAppearAnimationEvents() = default;
    virtual void ET_onAppearPlayed(EntityId viewId) = 0;
    virtual void ET_onDisappearPlayed(EntityId viewId) = 0;
};

#endif /* __ET_UI_ANIMATION_HPP__ */