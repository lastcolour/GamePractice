#include "Logics/UIPressAnimation.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Audio/ETSound.hpp"

void UIPressAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIPressAnimation>("UIPressAnimation")) {
        classInfo->addField("inDuration", &UIPressAnimation::inDuration);
        classInfo->addField("outDuration", &UIPressAnimation::outDuration);
        classInfo->addField("minScale", &UIPressAnimation::minScale);
        classInfo->addResourceField("sound", ResourceType::SoundEvent, &UIPressAnimation::setSoundEvent);
    }
}

UIPressAnimation::UIPressAnimation() :
    inDuration(0.075f),
    outDuration(0.075f),
    minScale(0.95f),
    currDuration(0.f),
    isReversed(false) {
}

UIPressAnimation::~UIPressAnimation() {
}

bool UIPressAnimation::init() {
    ETNode<ETUIAnimation>::connect(getEntityId());
    return true;
}

void UIPressAnimation::deinit() {
}

void UIPressAnimation::setSoundEvent(const char* soundName) {
    ET_SendEventReturn(soundEvent, &ETSoundManager::ET_createEvent, soundName);
}

void UIPressAnimation::ET_start() {
    currDuration = 0.f;
    isReversed = false;
    ET_SendEventReturn(startTm, getEntityId(), &ETEntity::ET_getTransform);
    ETNode<ETUITimerEvents>::connect(getEntityId());
    soundEvent.emit();
}

void UIPressAnimation::ET_startReverse() {
    currDuration = 0.f;
    isReversed = true;
    ET_SendEventReturn(startTm, getEntityId(), &ETEntity::ET_getTransform);
    ETNode<ETUITimerEvents>::connect(getEntityId());
}

void UIPressAnimation::ET_pause() {
    ETNode<ETUITimerEvents>::disconnect();
}

void UIPressAnimation::ET_resume() {
    ETNode<ETUITimerEvents>::connect(getEntityId());
}

void UIPressAnimation::ET_reverse() {
    isReversed = !isReversed;
}

bool UIPressAnimation::ET_isReversed() const {
    return isReversed;
}

float UIPressAnimation::ET_getDuration() const {
    return inDuration + outDuration;
}

void UIPressAnimation::ET_onUITick(float dt) {
    currDuration += dt;
    if(currDuration < inDuration) {
        auto currInTime = currDuration;
        auto prog = currInTime / inDuration;
        auto currScale = Math::Lerp(1.f, minScale, prog);
        auto currTm = startTm;
        currTm.scale *= currScale;
        ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, currTm);
    } else if (currDuration < outDuration) {
        auto currOutTime = currDuration - inDuration;
        auto prog = currOutTime / outDuration;
        auto currScale = Math::Lerp(minScale, 1.f, prog);
        auto currTm = startTm;
        currTm.scale *= currScale;
        ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, currTm);
    } else {
        ETNode<ETUITimerEvents>::disconnect();
        ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, startTm);
        ET_SendEvent(getEntityId(), &ETUIAnimationEvents::ET_onAnimationEnd);
    }
}