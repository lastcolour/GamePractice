#ifndef __UI_SURFACE_EVENT_HANDLER_HPP__
#define __UI_SURFACE_EVENT_HANDLER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"

class UISurfaceEventHandler : public SystemLogic,
    public ETNode<ETInputEvents> {
public:

    UISurfaceEventHandler();
    virtual ~UISurfaceEventHandler();

    // SystemLogic
    bool init() override;
    virtual void deinit() override;

    // ETSurfaceEvents
    void ET_onTouch(EActionType actionType, const Vec2i& pt) override;
    void ET_onButton(EActionType actionType, EButtonId buttonId) override;

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

#endif /* __UI_SURFACE_EVENT_HANDLER_HPP__ */