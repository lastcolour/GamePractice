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

struct ETUIViewManager {
    virtual ~ETUIViewManager() = default;
    virtual void ET_openView(EntityId viewId) = 0;
    virtual void ET_closeView(EntityId viewId) = 0;
};

struct ETUIEventManager {
    virtual ~ETUIEventManager() = default;
    virtual void ET_onEvent(UIEventType eventType) = 0;
};

#endif /* __ET_UI_VIEW_HPP__ */