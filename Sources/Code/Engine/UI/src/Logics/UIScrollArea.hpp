#ifndef __UI_SCROLL_AREA_HPP__
#define __UI_SCROLL_AREA_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIButton.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/UIScrollAreaStyle.hpp"
#include "UI/ETUITimer.hpp"
#include "UI/ETUIScrollArea.hpp"
#include "UI/ETUILayout.hpp"
#include "Core/TimePoint.hpp"

#include <vector>

class ReflectContext;

class UIScrollArea : public EntityLogic,
    public ETNode<ETUIInteractionBox>,
    public ETNode<ETUIElementEvents>,
    public ETNode<ETUITimerEvents>,
    public ETNode<ETUIScrollArea>,
    public ETNode<ETUIElemAligner> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIScrollArea();
    virtual ~UIScrollArea();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUIInteractionBox
    EInputEventResult ET_onInputEvent(EActionType type, const Vec2i& pt) override;

    // ETUIScrollArea
    void ET_setTarget(EntityId newTargetId) override;
    EntityId ET_getTarget() const override;
    void ET_setStyle(const UIScrollAreaStyle& newStyle) override;
    const UIScrollAreaStyle& ET_getStyle() const override;
    void ET_enableKinematicScroll(bool flag) override;
    float ET_getScrollProgress() const override;
    void ET_setScrollProgress(float newScrollProgress) override;
    void ET_setTargetPosClamped(const Vec2& newScrollPt) override;
    AABB2D ET_getScrollArea() const override;

    // ETUIElemAligner
    void ET_reAlign() override;

    // ETUIElementEvents
    void ET_onBoxChanged(const AABB2D& newAabb) override {}
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaChanged(float newAlpha) override;
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override;
    void ET_onIngoreTransform(bool flag) override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

private:

    void onPress(const Vec2& pt);
    bool onMove(const Vec2& pt);
    void onRelease(const Vec2& pt);
    void alignTarget();
    void updateMoveState(float dt);
    void addReleaseImpulse();
    void setPosUpdateProg(const AABB2D& scrollArea, const Vec2& newPt);
    void resetMoveState();

private:

    struct PathPoint {
        TimePoint timeP;
        Vec2 pt;
    };

    struct MoveState {
        Vec2 destPt;
        float vel;
        float acc;
        float force;
        float frameShift;
        bool reachDest;
    };

private:

    UIScrollAreaStyle style;
    std::vector<PathPoint> path;
    EntityId targetId;
    MoveState moveState;
    float scrollProgress;
    int extraZOffset;
    bool isPressed;
    bool kinematicScrollEnabled;
};

#endif /* __UI_SCROLL_AREA_HPP__ */