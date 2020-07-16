#ifndef __UI_BOX_HPP__
#define __UI_BOX_HPP__

#include "Logics/UIElement.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Entity/ETEntity.hpp"
#include "UI/UIBoxStyle.hpp"

class UIBox : public UIElement,
    public ETNode<ETUIBox>,
    public ETNode<ETRenderCameraEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIBox();
    virtual ~UIBox();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIElement
    AABB2Di ET_getBox() const override;
    UIBoxMargin ET_getMargin() const override;
    void ET_setAlpha(float newAlpha) override;

    // ETUIBox
    const UIBoxStyle& ET_getStyle() const override;
    void ET_setStyle(const UIBoxStyle& newBoxStyle) override;

    // ETRenderCameraEvents
    void ET_onRenderPortResized() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onAllLogicsCreated() override;

protected:

    void onZIndexChanged(int newZIndex) override;
    void onHide(bool flag) override;

private:

    void calculateBox();
    Vec2i calculateBoxSize();

private:

    AABB2Di aabb;
    UIBoxStyle style;
    EntityId boxRenderId;
    bool isVisible;
};

#endif /* __UI_BOX_HPP__ */