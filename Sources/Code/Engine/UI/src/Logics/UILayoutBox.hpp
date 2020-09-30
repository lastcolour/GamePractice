#ifndef __UI_LAYOUT_BOX_HPP__
#define __UI_LAYOUT_BOX_HPP__

#include "Logics/UIElement.hpp"
#include "UI/UIBoxStyle.hpp"
#include "UI/ETUILayout.hpp"

class UILayoutBox : public UIElement,
    public ETNode<ETUILayoutEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILayoutBox();
    virtual ~UILayoutBox();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIElement
    AABB2Di ET_getBox() const override;
    UIBoxMargin ET_getMargin() const override;

    // ETUILayoutEvents
    void ET_onLayoutChanged(const AABB2Di& newCombinedBox) override;

    // ETEntityEvents
    void ET_onAllLogicsCreated() override;

protected:

    void onZIndexChanged(int newZIndex) override;
    void onHide(bool flag) override;
    void onAlphaChanged(float newAlpha) override;
    void onTransformChanged(const Transform& newTm) override;
    void onDisabled(bool flag) override {}

private:

    AABB2Di aabb;
    UIBoxStyle::Margin styleMargin;
    EntityId boxRenderId;
};

#endif /* __UI_LAYOUT_BOX_HPP__ */