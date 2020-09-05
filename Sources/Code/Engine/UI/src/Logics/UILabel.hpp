#ifndef __UI_LABEL_HPP__
#define __UI_LABEL_HPP__

#include "UI/ETUIViewPort.hpp"
#include "Logics/UIElement.hpp"
#include "Logics/UILabelStyle.hpp"

class ReflectContext;

class UILabel : public UIElement,
    public ETNode<ETUILabel>,
    public ETNode<ETUIViewPortEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILabel();
    virtual ~UILabel();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUILabel
    void ET_setText(const char* newText) override;
    const char* ET_getText() const override;
    float ET_getFontSize() const override;
    void ET_setFontSize(float newSize) override;
    void ET_setTextRender(EntityId newRenderId) override;

    // ETUIElement
    AABB2Di ET_getBox() const override;
    UIBoxMargin ET_getMargin() const override;

    // ETUIViewPortEvents
    void ET_onViewPortChanged(const Vec2i& newSize) override;

    // ETEntityEvents
    void ET_onAllLogicsCreated() override;

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