#include "Logics/UIPressAnimation.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Audio/ETSound.hpp"
#include "UIUtils.hpp"

void UIPressAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIPressAnimation>("UIPressAnimation")) {
        classInfo->addField("duration", &UIPressAnimation::duration);
        classInfo->addField("minScale", &UIPressAnimation::minScale);
        classInfo->addResourceField("sound", ResourceType::SoundEvent, &UIPressAnimation::setSoundEvent);
    }
}

UIPressAnimation::UIPressAnimation() :
    duration(0.075f),
    minScale(0.95f),
    currDuration(0.f) {
}

UIPressAnimation::~UIPressAnimation() {
}

bool UIPressAnimation::init() {
    ETNode<ETUIPressAnimation>::connect(getEntityId());
    return true;
}

void UIPressAnimation::deinit() {
}

void UIPressAnimation::setSoundEvent(const char* soundName) {
    ET_SendEventReturn(soundEvent, &ETSoundManager::ET_createEvent, soundName);
}

void UIPressAnimation::ET_playPress(EntityId triggerId) {
    currDuration = 0.f;
    senderId = triggerId;
    ET_SendEventReturn(startTm, getEntityId(), &ETEntity::ET_getLocalTransform);
    ETNode<ETUITimerEvents>::connect(getEntityId());
    soundEvent.emit();
}

float UIPressAnimation::getTotalDuration() const {
    return (1.f + 1.f * UI::ReturnAnimScaleFactor) * duration;
}

void UIPressAnimation::ET_onUITick(float dt) {
    currDuration += dt;
    if(currDuration < getTotalDuration()) {
        float prog = 1.f;
        if(currDuration < duration) {
            prog = currDuration / duration;
        } else {
            prog = 1.f -  UI::ReturnAnimScaleFactor * (currDuration - duration) / duration;
        }
        auto scaleFactor = Math::Lerp(1.f, minScale, prog);
        auto tm = startTm;
        tm.scale *= scaleFactor;
        UI::SetLocalTMDoNotUpdateLayout(getEntityId(), tm);
        ET_SendEvent(getEntityId(), &ETEntity::ET_setLocalTransform, tm);
    } else {
        ETNode<ETUITimerEvents>::disconnect();
        UI::SetLocalTMDoNotUpdateLayout(getEntityId(), startTm);
        ET_SendEvent(senderId, &ETUIPressAnimationEvents::ET_onPressPlayed);
    }
}