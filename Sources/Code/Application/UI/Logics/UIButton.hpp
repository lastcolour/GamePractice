#ifndef __UI_BUTTON_HPP__
#define __UI_BUTTON_HPP__

#include "UI/Logics/UIBox.hpp"

class UIButton : public UIBox,
    public ETNode<ETUIButton> {
public:

    UIButton();
    virtual ~UIButton();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIButton
    void ET_onPress() override;

private:

    std::string eventName;
    EntityId labelEntId;
};

#endif /* __UI_BUTTON_HPP__ */