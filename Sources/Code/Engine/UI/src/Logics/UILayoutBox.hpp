#ifndef __UI_LAYOUT_BOX_HPP__
#define __UI_LAYOUT_BOX_HPP__

#include "Logics/UIElement.hpp"
#include "UI/UIBoxStyle.hpp"
#include "UI/ETUILayout.hpp"

class UILayoutBox : public UIElement,
    public ETNode<ETUILayoutEvents>,
    public ETNode<ETUIElementGeom> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILayoutBox();
    virtual ~UILayoutBox();

    // EntityLogic
    void init() override;
    void onLoaded() override;

    // ETUIElementGeom
    void ET_setRenderId(EntityId newRenderId) override;
    EntityId ET_getRenderId(EntityId newRenderId) const override;

    // ETUIElementGeom
    AABB2D ET_getBox() const override;
    UIBoxMargin ET_getMargin() const override;

    // ETUILayoutEvents
    void ET_onLayoutChanged(const AABB2D& newCombinedBox) override;

protected:

    void onZIndexChanged(int newZIndex) override;
    void onHide(bool flag) override;
    void onAlphaChanged(float newAlpha) override;
    void onTransformChanged(const Transform& newTm) override {}
    void onDisabled(bool flag) override {}

private:

    AABB2D aabb;
    UIBoxStyle::Margin styleMargin;
    EntityId boxRenderId;
};

#endif /* __UI_LAYOUT_BOX_HPP__ */