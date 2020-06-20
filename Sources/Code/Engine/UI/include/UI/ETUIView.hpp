#ifndef __ET_UI_VIEW_HPP__
#define __ET_UI_VIEW_HPP__

#include "Core/Core.hpp"

struct ETUIViewStack {
    virtual ~ETUIViewStack() = default;
    virtual void ET_pushView(const char* viewName) = 0;
    virtual void ET_forcePushView(const char* viewName) = 0;
    virtual void ET_clearAllAndPushNewView(const char* viewName) = 0;
    virtual void ET_popView() = 0;
    virtual void ET_forceReset() = 0;
    virtual EntityId ET_getActiveViewId() const = 0;
};

struct ETUIViewStackEvents {
    virtual ~ETUIViewStackEvents() = default;
    virtual void ET_onViewStartPush(EntityId viewId) = 0;
    virtual void ET_onViewFinishPush(EntityId viewId) = 0;
    virtual void ET_onViewStartPop(EntityId viewId) = 0;
    virtual void ET_onViewFinishPop(EntityId viewId) = 0;
};

struct ETUIViewSwitcher {
    virtual ~ETUIViewSwitcher() = default;
    virtual void ET_reverseSwitchView(EntityId newViewId, EntityId oldViewId) = 0;
    virtual void ET_swtichView(EntityId newViewId, EntityId oldViewId) = 0;
    virtual void ET_forceSwtichStop() = 0;
    virtual void ET_reverse() = 0;
    virtual float ET_getTotalSwitchDuration() const = 0;
};

struct ETUIViewSwitcherEvents {
    virtual ~ETUIViewSwitcherEvents() = default;
    virtual void ET_onViewSwitchFinished(EntityId viewId) = 0;
};

struct ETUIAppearAnimation {
    virtual ~ETUIAppearAnimation() = default;
    virtual bool ET_isAppearing() const = 0;
    virtual void ET_setAppear(bool flag) = 0;
    virtual bool ET_animate(float duration) = 0;
    virtual bool ET_isNeedHideOldView() const = 0;
    virtual float ET_getDuration() const = 0;
};

struct ETUIEventManager {
    virtual ~ETUIEventManager() = default;
    virtual void ET_onEvent(const char* eventName) = 0;
};

#endif /* __ET_UI_VIEW_HPP__ */