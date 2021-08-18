#ifndef __UI_LABEL_HPP__
#define __UI_LABEL_HPP__

#include "UI/ETUIViewPort.hpp"
#include "Logics/UIElement.hpp"

class UILabel : public UIElement,
    public ETNode<ETUILabel>,
    public ETNode<ETUIViewPortEvents>,
    public ETNode<ETUIElementGeom> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILabel();
    virtual ~UILabel();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUIElementGeom
    void ET_setRenderId(EntityId newRenderId) override;
    EntityId ET_getRenderId(EntityId newRenderId) const override;

    // ETUILabel
    void ET_setText(const char* newText) override;
    const char* ET_getText() const override;
    const UILabelStyle& ET_getStyle() const override;
    void ET_setStyle(const UILabelStyle& newStyle) override;

    // ETUIElementGeom
    AABB2D ET_getBox() const override;
    UIBoxMargin ET_getMargin() const override;

    // ETUIViewPortEvents
    void ET_onViewPortChanged(const Vec2i& newSize) override;

    // ETEntityEvents
    void ET_onLoaded() override;

protected:

    void onZIndexChanged(int newZIndex) override;
    void onHide(bool flag) override;
    void onAlphaChanged(float newAlpha) override;
    void onDisabled(bool flag) override {}
    void onTransformChanged(const Transform& newTm) override {}

private:

    UILabelStyle style;
    std::string text;
    EntityId labelRenderId;
};

#endif /* __UI_LABEL_HPP__ */