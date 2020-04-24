#include "UI/Logics/UIButtonPressAnimation.hpp"
#include "Core/JSONNode.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "Audio/ETAudioInterfaces.hpp"

#include <algorithm>

UIButtonPressAnimation::UIButtonPressAnimation() :
    startScale(1.f),
    inDuration(0.1f),
    outDuration(0.1f),
    minScale(0.8f),
    progress(-1.f) {
}

UIButtonPressAnimation::~UIButtonPressAnimation() {
}

bool UIButtonPressAnimation::serialize(const JSONNode& node) {
    node.read("inDuration", inDuration);
    node.read("outDuration", outDuration);
    node.read("minScale", minScale);
    return true;
}

bool UIButtonPressAnimation::init() {
    ETNode<ETUIButtonPressAnimation>::connect(getEntityId());
    return true;
}

void UIButtonPressAnimation::ET_onTick(float dt) {
    progress += dt;

    Transform tm;
    ET_SendEventReturn(tm, getParentId(), &ETEntity::ET_getTransform);

    if(progress < inDuration) {
        float lerpProg = std::min(progress / inDuration, 1.f);
        float resScale = Math::Lerp(1.f, minScale, lerpProg);
        tm.scale = resScale * startScale;
        ET_SendEvent(getParentId(), &ETEntity::ET_setTransform, tm);
    } else {
        float outProgress = progress - inDuration;
        if(outProgress < outDuration) {
            float lerpProg = std::min(outProgress / outDuration, 1.f);
            float resScale = Math::Lerp(minScale, 1.f, lerpProg);
            tm.scale = resScale * startScale;
            ET_SendEvent(getParentId(), &ETEntity::ET_setTransform, tm);
        } else {
            ETNode<ETTimerEvents>::disconnect();
            progress = -1.f;
            tm.scale = startScale;
            ET_SendEvent(getParentId(), &ETEntity::ET_setTransform, tm);
            ET_SendEvent(getParentId(), &ETUIButtonPressAnimationEvents::ET_onPressAnimationEnd);
        }
    }
}

void UIButtonPressAnimation::ET_startPressAnimation() {
    if(progress >= 0.f) {
        LogWarning("[UIButtonPressAnimation::ET_startPressAnimation] Double start of press animation");
        return;
    }
    Transform tm;
    ET_SendEventReturn(tm, getParentId(), &ETEntity::ET_getTransform);
    ET_SendEvent(getEntityId(), &ETSoundPlayer::ET_play);
    startScale = tm.scale;
    progress = 0.f;

    ETNode<ETTimerEvents>::connect(getEntityId());
}

float UIButtonPressAnimation::ET_getAnimationDuration() const {
    return inDuration + outDuration;
}