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
    bool init() override;
    void deinit() override;

    // ETUIInteractionBox
    EInputEventResult ET_onInputEvent(EActionType type, const Vec2i& pt) override;
    AABB2Di ET_getHitBox() const override;

    // ETUIScrollArea
    void ET_setTarget(EntityId newTargetId) override;
    EntityId ET_getTarget() const override;
    void ET_setStyle(const UIScrollAreaStyle& newStyle) override;
    const UIScrollAreaStyle& ET_getStyle() const override;

    // ETUIElemAligner
    void ET_reAlign() override;

    // ETUIElementEvents
    void ET_onBoxChanged(const AABB2Di& newAabb) override;
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaChanged(float newAlpha) override;
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override;
    void ET_onIngoreTransform(bool flag) override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

private:

    void onPress(const Vec2i& pt);
    bool onMove(const Vec2i& pt);
    void onRelease(const Vec2i& pt);
    void initScrollElem();

private:

    struct PathPoint {
        TimePoint timeP;
        Vec2i pt;
    };

private:

    UIScrollAreaStyle style;
    std::vector<PathPoint> path;
    EntityId targetId;
    AABB2Di scrollBox;
    Vec2i endScrollPt;
    float scrollSpeed;
    float accumulativeDt;
    bool isPressed;
};

#endif /* __UI_SCROLL_AREA_HPP__ */