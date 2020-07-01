#ifndef __UI_LABEL_HPP__
#define __UI_LABEL_HPP__

#include "Logics/UIElement.hpp"
#include "Render/ETRenderCamera.hpp"

class ReflectContext;

class UILabel : public UIElement,
    public ETNode<ETUILabel>,
    public ETNode<ETUIVisibleElement>,
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
    virtual AABB2Di ET_getBox() const override;
    virtual UIBoxMargin ET_getMargin() const override;

    // ETUIVisibleElement
    void ET_show() override;
    void ET_hide() override;
    bool ET_isVisible() const override;

    // ETRenderCameraEvents
    void ET_onRenderPortResized() override;

    // ETEntityEvents
    void ET_onAllLogicsCreated() override;

protected:

    void onZIndexChanged(int newZIndex) override;

private:

    float fontSize;
    std::string text;
    EntityId labelRenderId;
};

#endif /* __UI_LABEL_HPP__ */