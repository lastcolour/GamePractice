#ifndef __UI_LABEL_HPP__
#define __UI_LABEL_HPP__

#include "Logics/UIElement.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Logics/UILabelStyle.hpp"

class ReflectContext;

class UILabel : public UIElement,
    public ETNode<ETUILabel>,
    public ETNode<ETRenderCameraEvents> {
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
    void ET_show() override;
    void ET_hide() override;
    bool ET_isVisible() const override;
    void ET_setAlpha(float newAlpha) override;

    // ETRenderCameraEvents
    void ET_onRenderPortResized() override;

    // ETEntityEvents
    void ET_onAllLogicsCreated() override;

protected:

    void onZIndexChanged(int newZIndex) override;

private:

    UILabelStyle style;
    std::string text;
    EntityId labelRenderId;
};

#endif /* __UI_LABEL_HPP__ */