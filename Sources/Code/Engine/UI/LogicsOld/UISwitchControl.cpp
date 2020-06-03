#include "UI/Logics/UISwitchControl.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

#include <cassert>

UISwitchControl::UISwitchControl() :
    onLabel("On"),
    offLabel("Off"),
    controlType(EControlType::Sound),
    isTurnedOn(false) {
}

UISwitchControl::~UISwitchControl() {
}

void UISwitchControl::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EControlType>("UISwtichControlType")) {
        enumInfo->addValues<EControlType>({
            {"Sound", EControlType::Sound},
            {"Vibration", EControlType::Vibration}
        });
    }
    if(auto classInfo = ctx.classInfo<UISwitchControl>("UISwitchControl")) {
        classInfo->addField("onLabel", &UISwitchControl::onLabel);
        classInfo->addField("offLabel", &UISwitchControl::offLabel);
        classInfo->addField("isTurnedOn", &UISwitchControl::isTurnedOn);
        classInfo->addField("controlType", &UISwitchControl::controlType);
    }
}

bool UISwitchControl::init() {
    switch(controlType)
    {
    case EControlType::Sound: {
        ET_SendEventReturn(isTurnedOn, &ETGameConfig::ET_isSoundEnabled);
        break;
    }
    case EControlType::Vibration: {
        ET_SendEventReturn(isTurnedOn, &ETGameConfig::ET_isVibrationEnabled);
        break;
    }
    default:
        assert(false && "Invalid control type");
        return false;
    }

    setLabel();
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    ETNode<ETUISwitchControl>::connect(getEntityId());
    return true;
}

void UISwitchControl::deinit() {
}

void UISwitchControl::setLabel() {
    if(isTurnedOn) {
        ET_SendEvent(getEntityId(), &ETUILabeledBox::ET_setLabelText, onLabel.c_str());
    } else {
        ET_SendEvent(getEntityId(), &ETUILabeledBox::ET_setLabelText, offLabel.c_str());
    }
}

void UISwitchControl::updateControl() {
    switch(controlType)
    {
    case EControlType::Sound: {
        ET_SendEvent(&ETGameConfig::ET_setSoundEnabled, isTurnedOn);
        break;
    }
    case EControlType::Vibration: {
        ET_SendEvent(&ETGameConfig::ET_setVibrationEnabled, isTurnedOn);
        break;
    }
    default:
        assert(false && "Invalid control type");
    }
}

void UISwitchControl::ET_onPress() {
    isTurnedOn = !isTurnedOn;
    updateControl();
    setLabel();
}

void UISwitchControl::ET_onHover(bool flag) {
    (void)flag;
}

bool UISwitchControl::ET_isHovered() const {
    return false;
}

AABB2Di UISwitchControl::ET_getHitBox() const {
    AABB2Di box;
    ET_SendEventReturn(box, getEntityId(), &ETUIBox::ET_getAabb2di);
    return box;
}

bool UISwitchControl::ET_isEnabled() const {
    return isTurnedOn;
}
