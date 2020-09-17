#include "Logics/UIGameEndStarsAppearAnimation.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntity.hpp"

#include <cassert>
#include <algorithm>

namespace {

EntityId getStartForStage(int stage, EntityId first, EntityId second, EntityId third) {
    if(stage == 0) {
        return first;
    } else if(stage == 1) {
        return second;
    } else if(stage == 2) {
        return third;
    }
    assert(false && "Invalid stage");
    return InvalidEntityId;
}

} // namespace

void UIGameEndStarsAppearAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIGameEndStarsAppearAnimation>("UIGameEndStarsAppearAnimation")) {
        classInfo->addField("firstStar", &UIGameEndStarsAppearAnimation::firstId);
        classInfo->addField("secondStar", &UIGameEndStarsAppearAnimation::secondId);
        classInfo->addField("thirdStar", &UIGameEndStarsAppearAnimation::thirdId);
        classInfo->addField("duration", &UIGameEndStarsAppearAnimation::duration);
        classInfo->addField("startScale", &UIGameEndStarsAppearAnimation::startScale);
    }
}

UIGameEndStarsAppearAnimation::UIGameEndStarsAppearAnimation() :
    duration(0.2f),
    startScale(2.f),
    currentDuration(0.f),
    currentStage(0),
    endStage(0) {
}

UIGameEndStarsAppearAnimation::~UIGameEndStarsAppearAnimation() {
}

bool UIGameEndStarsAppearAnimation::init() {
    ETNode<ETUIStarsAppearAnimation>::connect(getEntityId());
    return true;
}

void UIGameEndStarsAppearAnimation::deinit() {
}

void UIGameEndStarsAppearAnimation::ET_onUITick(float dt) {
    auto starId = getStartForStage(currentStage, firstId, secondId, thirdId);
    currentDuration += dt;
    if(currentDuration >= duration) {
        ET_SendEvent(starId, &ETEntity::ET_setTransform, starStartTm);
        ++currentStage;
        if(currentStage == endStage) {
            currentStage = 0;
            currentDuration = 0.0;
            ETNode<ETUITimerEvents>::disconnect();
            ET_SendEvent(&ETUIStarsAppearAnimationEvents::ET_onStarsAppeared);
            return;
        } else {
            starId = getStartForStage(currentStage, firstId, secondId, thirdId);
            ET_SendEventReturn(starStartTm, starId, &ETEntity::ET_getTransform);
            currentDuration -= duration;
        }
    }

    if(!starId.isValid()) {
        return;
    }

    auto prog = std::min(1.f, currentDuration / duration);
    Transform newTm = starStartTm;
    auto currScale = Math::Lerp(startScale, 1.f, prog);
    newTm.scale *= currScale;
    ET_SendEvent(starId, &ETEntity::ET_setTransform, newTm);
}

void UIGameEndStarsAppearAnimation::ET_showStars(int starsCount) {
    if(currentStage != 0) {
        LogError("[UIGameEndStarsAppearAnimation::ET_start] Can't re-start animation in progress");
        return;
    }
    if(starsCount < 1 || starsCount > 3) {
        LogError("[UIGameEndStarsAppearAnimation::ET_showStars] Invalid amount of stars to show: %d (Should be in range [1, 3])", starsCount);
        assert(false && "Invalid stars count");
        return;
    }

    endStage = starsCount;

    auto starId = getStartForStage(currentStage, firstId, secondId, thirdId);
    ET_SendEventReturn(starStartTm, starId, &ETEntity::ET_getTransform);

    ETNode<ETUITimerEvents>::connect(getEntityId());
}