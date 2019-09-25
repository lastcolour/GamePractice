#include "UI/Logics/UISwitchControl.hpp"
#include "Core/JSONNode.hpp"
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

bool UISwitchControl::serialize(const JSONNode& node) {
    node.read("onLabel", onLabel);
    node.read("offLabel", offLabel);

    std::string controlTypeStr;
    node.read("controlType", controlTypeStr);
    if(controlTypeStr == "sound") {
        controlType = EControlType::Sound;
    } else if(controlTypeStr == "vibration") {
        controlType = EControlType::Vibration;
    } else {
        LogWarning("[UISwitchControl::serialize] Unknown control type: %s", controlTypeStr);
        return false;
    }
    return true;
}

bool UISwitchControl::init() {
    switch (controlType)
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

void UISwitchControl::setLabel() {
    if(isTurnedOn) {
        ET_SendEvent(getEntityId(), &ETUILabeledBox::ET_setLabelText, onLabel.c_str());
    } else {
        ET_SendEvent(getEntityId(), &ETUILabeledBox::ET_setLabelText, offLabel.c_str());
    }
}

void UISwitchControl::updateControl() {
    switch (controlType)
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
