#ifndef __UI_SURFACE_TOUCH_MANAGER_HPP__
#define __UI_SURFACE_TOUCH_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"

class UISurfaceTouchManager : public SystemLogic,
    public ETNode<ETInputEvents> {
public:

    UISurfaceTouchManager();
    virtual ~UISurfaceTouchManager();

    // SystemLogic
    bool init() override;
    virtual void deinit() override;

    // ETSurfaceEvents
    void ET_onTouch(ETouchType touchType, const Vec2i& pt) override;

private:

    void onPress(const Vec2i& pt);
    void onMove(const Vec2i& pt);
    void onRelease(const Vec2i& pt);

    bool isHover(const Vec2i& pt, EntityId entId) const;
    EntityId getHoveredEntity(const Vec2i& pt) const;

private:

    EntityId pressElemId;
    EntityId hoveredElemId;
};

#endif /* __UI_SURFACE_TOUCH_MANAGER_HPP__ */