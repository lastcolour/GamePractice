#include "Logics/UIPressAnimation.hpp"
#include "Reflect/ReflectContext.hpp"

void UIPressAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIPressAnimation>("UIPressAnimation")) {
        classInfo->addField("inDuration", &UIPressAnimation::inDuration);
        classInfo->addField("outDuration", &UIPressAnimation::outDuration);
        classInfo->addField("minScale", &UIPressAnimation::minScale);
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

void UIPressAnimation::ET_start() {
    currDuration = 0.f;
    isReversed = false;
    ETNode<ETAppTimerEvents>::connect(getEntityId());
}

void UIPressAnimation::ET_startReverse() {
    currDuration = 0.f;
    isReversed = true;
    ETNode<ETAppTimerEvents>::connect(getEntityId());
}

void UIPressAnimation::ET_pause() {
}

void UIPressAnimation::ET_resume() {
}

void UIPressAnimation::ET_reverse() {
}

bool UIPressAnimation::ET_isReversed() const {
}

float UIPressAnimation::ET_getDuration() const {
}

void UIPressAnimation::ET_onAppTick(float dt) {
}