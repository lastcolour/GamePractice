#ifndef __UI_ET_INTERFACES_HPP__
#define __UI_ET_INTERFACES_HPP__

#include "Math/AABB.hpp"
#include "Core/ETPrimitives.hpp"

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

struct ETUILabel {
    virtual ~ETUILabel() = default;
    virtual void ET_setText(const char* text) = 0;
    virtual const char* ET_getText() const = 0;
};

struct ETUIImage {
    virtual ~ETUIImage() = default;
    virtual void ET_setImage(const char* image) = 0; 
};

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

struct ETUIButtonEventManager {
    virtual ~ETUIButtonEventManager() = default;
    virtual bool ET_isSomeButtonPressed() const = 0;
    virtual void ET_setButtonPressed(bool flag) = 0;
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

struct ETUIEventManager {
    virtual ~ETUIEventManager() = default;
    virtual void ET_onEvent(const char* eventName) = 0;
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

struct ETUISwitchControl {
    virtual ~ETUISwitchControl() = default;
    virtual bool ET_isEnabled() const = 0;
};

struct ETUIAppearAnimation {
    virtual ~ETUIAppearAnimation() = default;
    virtual bool ET_isAppearing() const = 0;
    virtual void ET_setAppear(bool flag) = 0;
    virtual bool ET_animate(float duration) = 0;
    virtual bool ET_isNeedHideOldView() const = 0;
    virtual float ET_getDuration() const = 0;
};

#endif /* __UI_ET_INTERFACES_HPP__ */