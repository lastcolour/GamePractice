#ifndef __ET_UI_VIEW_HPP__
#define __ET_UI_VIEW_HPP__

enum class UIViewType {
    None = 0,
    Main,
    Game,
    EndGame,
    PauseGame,
    Background,
    Levels,
    Loading
};

struct ETUIView {
    virtual ~ETUIView() = default;
    virtual void ET_setFocus(bool flag) = 0;
    virtual bool ET_getFocus() const = 0;
};

struct ETUIViewManager {
    virtual ~ETUIViewManager() = default;
    virtual bool ET_openView(UIViewType viewType) = 0;
    virtual void ET_closeView(UIViewType viewType) = 0;
    virtual UIViewType ET_getActiveViewType() const = 0;
    virtual EntityId ET_getActiveViewId() const = 0;
    virtual void ET_onViewLoaded(UIViewType viewType, EntityId viewId) = 0;
    virtual void ET_onViewStartAppearing(EntityId viewId) = 0;
    virtual void ET_onViewAppeared(EntityId viewId) = 0;
    virtual void ET_onViewDisappeared(EntityId viewId) = 0;
};

struct ETUIViewTransitionManager {
    virtual ~ETUIViewTransitionManager() = default;
    virtual void ET_addAppearing(EntityId viewId) = 0;
    virtual void ET_addDisappearing(EntityId viewId) = 0;
    virtual bool ET_hasActiveTransition() const = 0;
};

struct ETUIReAlignManager {
    virtual ~ETUIReAlignManager() = default;
    virtual void ET_setLayoutDirty(EntityId entId) = 0;
    virtual void ET_doReAlign() = 0;
};

#endif /* __ET_UI_VIEW_HPP__ */