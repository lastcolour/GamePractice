#ifndef __UI_BUTTON_HPP__
#define __UI_BUTTON_HPP__

#include "UI/Logics/UIBox.hpp"
#include "ETApplicationInterfaces.hpp"

class UIButton : public UIBox,
    public ETNode<ETUIButton>,
    public ETNode<ETUIInteractionBox>,
    public ETNode<ETUIButtonPressAnimationEvents> {
public:

    UIButton();
    virtual ~UIButton();

    // UIBox
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIInteractionBox
    AABB2Di ET_getHitBox() const override;
    void ET_onPress() override;
    void ET_onHover(bool flag) override;
    bool ET_isHovered() const override;

    // ETUIButton
    float ET_getPressDuration() const override;
    void ET_setEventName(const char* newEventName) override;

    // ETUIButtonPressAnimationEvents
    void ET_onPressAnimationEnd() override;

private:

    std::string eventName;
    EntityId animEntityId;
    bool isHovered;
};

#endif /* __UI_BUTTON_HPP__ */