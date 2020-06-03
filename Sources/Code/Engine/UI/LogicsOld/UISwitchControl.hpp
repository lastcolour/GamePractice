#ifndef __UI_SWITCH_CONTROL_HPP__
#define __UI_SWITCH_CONTROL_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIInterfaces.hpp"

class UISwitchControl : public EntityLogic,
    public ETNode<ETUIInteractionBox>,
    public ETNode<ETUISwitchControl> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    enum class EControlType {
        Sound = 0,
        Vibration
    };

public:

    UISwitchControl();
    virtual ~UISwitchControl();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIInteractionBox
    void ET_onPress() override;
    void ET_onHover(bool flag) override;
    bool ET_isHovered() const override;
    AABB2Di ET_getHitBox() const override;

    // ETUISwitchControl
    bool ET_isEnabled() const override;

private:

    void setLabel();
    void updateControl();

private:

    std::string onLabel;
    std::string offLabel;
    EControlType controlType;
    bool isTurnedOn;
};

#endif /* __UI_SWITCH_CONTROL_HPP__ */