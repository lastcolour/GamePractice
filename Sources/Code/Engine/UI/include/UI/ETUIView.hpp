#ifndef __ET_UI_VIEW_HPP__
#define __ET_UI_VIEW_HPP__

#include "Core/Core.hpp"

enum class UIEventType {
    None = 0,
    OnStartGame,
    OnBackButton,
    OnGameEnd,
    OnSurfaceHidden,
    OnSurfaceShown
};

enum class UIViewType {
    None = 0,
    Main,
    Game
};

struct ETUIViewManager {
    virtual ~ETUIViewManager() = default;
    virtual bool ET_openView(UIViewType viewType) = 0;
    virtual void ET_closeView(UIViewType viewType) = 0;
    virtual UIViewType ET_getActiveViewType() const = 0;
    virtual EntityId ET_getActiveViewId() const = 0;
};

struct ETUIEventManager {
    virtual ~ETUIEventManager() = default;
    virtual void ET_onEvent(UIEventType eventType) = 0;
};

struct ETUIViewAppearAnimation {
    virtual ~ETUIViewAppearAnimation() = default;
    virtual void ET_appear() = 0;
    virtual void ET_disappear() = 0;
};

struct ETUIViewAppearAnimationEvents {
    virtual ~ETUIViewAppearAnimationEvents() = default;
    virtual void ET_onViewAppeared(EntityId viewId) = 0;
    virtual void ET_onViewDisappeared(EntityId viewId) = 0;
};

#endif /* __ET_UI_VIEW_HPP__ */