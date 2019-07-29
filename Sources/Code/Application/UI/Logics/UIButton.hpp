#ifndef __UI_BUTTON_HPP__
#define __UI_BUTTON_HPP__

#include "UI/Logics/UIBox.hpp"

class UIButton : public UIBox,
    public ETNode<ETUIButton>,
    public ETNode<ETUIInteractionBox> {
public:

    UIButton();
    virtual ~UIButton();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIInteractionBox
    const AABB2Di& ET_getHitBox() const override;
    void ET_onPress() override;
    void ET_setEventName(const char* newEventName) override;
    void ET_onHover(bool flag) override;
    bool ET_isHovered() const override;

private:

    std::string eventName;
    EntityId labelEntId;
    bool isHovered;
};

#endif /* __UI_BUTTON_HPP__ */