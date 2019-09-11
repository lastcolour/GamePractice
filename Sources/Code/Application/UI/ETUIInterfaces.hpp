#ifndef __UI_ET_INTERFACES_HPP__
#define __UI_ET_INTERFACES_HPP__

#include "Math/AABB.hpp"
#include "Core/ETPrimitives.hpp"

struct UIStyle;

struct Margin {
    int top;
    int bot;
    int left;
    int right;

    Margin() : 
        top(0), bot(0), left(0), right(0) {}
};

enum class UIListType {
    Vertical = 0,
    Horizontal
};

struct ETUIBoxEvents {
    virtual ~ETUIBoxEvents() = default;
    virtual void ET_onBoxResized() = 0;
};

struct ETUIBox {
    virtual ~ETUIBox() = default;
    virtual const AABB2Di& ET_getAabb2di() const = 0;
    virtual void ET_setCenter(const Vec2i& pos) = 0;
    virtual void ET_alignInBox(const AABB2Di& alignBox) = 0;
    virtual void ET_boxResize() = 0;
    virtual const UIStyle& ET_getStyle() const = 0;
    virtual void ET_setStyle(const UIStyle& newStyle) = 0;
    virtual const Margin& ET_getMaring() const = 0;
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

struct ETUIList {
    virtual ~ETUIList() = default;
    virtual void ET_setType(UIListType listType) = 0;
};

struct ETUIViewStack {
    virtual ~ETUIViewStack() = default;
    virtual void ET_pushView(const char* viewName) = 0;
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
    virtual float ET_getSwitchDuration() const = 0;
    virtual void ET_reverseSwitchView(EntityId newViewId, EntityId oldViewId) = 0;
    virtual void ET_swtichView(EntityId newViewId, EntityId oldViewId) = 0;
    virtual void ET_forceSwtichStop() = 0;
    virtual void ET_reverse() = 0;
};

struct ETUIViewSwitcherEvents {
    virtual ~ETUIViewSwitcherEvents() = default;
    virtual void ET_onViewSwitchFinished(EntityId viewId) = 0;
};

struct ETUIEventManager {
    virtual ~ETUIEventManager() = default;
    virtual void ET_onEvent(const char* eventName) = 0;
};

struct ETUIConfigManager {
    virtual ~ETUIConfigManager() = default;
    virtual const char* ET_getDefaultGeomRenderer() = 0;
    virtual const char* ET_getDefaultTextRenderer() = 0;
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

#endif /* __UI_ET_INTERFACES_HPP__ */