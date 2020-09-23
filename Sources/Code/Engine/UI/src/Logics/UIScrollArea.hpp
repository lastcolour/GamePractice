#ifndef __UI_SCROLL_AREA_HPP__
#define __UI_SCROLL_AREA_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIButton.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/UIScrollAreaStyle.hpp"

#include <vector>

class ReflectContext;

class UIScrollArea : public EntityLogic,
    public ETNode<ETUIInteractionBox>,
    public ETNode<ETUIElementEvents> {
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

    // ETUIElementEvents
    void ET_onBoxResized(const AABB2Di& newAabb) override;
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaChanged(float newAlpha) override;
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override;
    void ET_onIngoreTransform(bool flag) override;

private:

    void onPress(const Vec2i& pt);
    void onMove(const Vec2i& pt);
    void onRelease(const Vec2i& pt);
    void initScrollElem();

private:

    UIScrollAreaStyle style;
    std::vector<Vec2i> path;
    EntityId scrollElemId;
    float currentProg;
    Vec2i startPt;
    Vec2i endPt;
};

#endif /* __UI_SCROLL_AREA_HPP__ */