#ifndef __UI_BUTTON_HPP__
#define __UI_BUTTON_HPP__

#include "Logics/UIBoxVisual.hpp"
#include "UI/ETUIButton.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIAnimation.hpp"

class ReflectContext;

class UIButton : public UIBoxVisual,
    public ETNode<ETUIInteractionBox>,
    public ETNode<ETUIAnimationEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIButton();
    virtual ~UIButton();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIInteractionBox
    void ET_onPress() override;
    void ET_onHover(bool flag) override;
    AABB2Di ET_getHitBox() const override;
    bool ET_isHovered() const override;

    // ETUIAnimationEvents
    void ET_onAnimationEnd() override;

private:

    EntityId labelId;
    UIEventType eventType;
    bool isHovered;
};

#endif /* __UI_BUTTON_HPP__ */