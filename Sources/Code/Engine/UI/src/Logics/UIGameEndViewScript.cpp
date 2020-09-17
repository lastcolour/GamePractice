#include "Logics/UIGameEndViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGameScore.hpp"

#include <cassert>

namespace {

int covertToStarsCount(ObjectiveProgress objectiveProgress) {
    int stars = 0;
    switch(objectiveProgress) {
        case ObjectiveProgress::Fail:
            break;
        case ObjectiveProgress::OneStar:
            stars = 1;
            break;
        case ObjectiveProgress::TwoStars:
            stars = 2;
            break;
        case ObjectiveProgress::ThreeStars:
            stars = 3;
            break;
        default:
            assert(false && "Invalid objective progress");
    }
    return stars;
}

} // namespace

void UIGameEndViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIGameEndViewScript>("UIGameEndViewScript")) {
    }
}

UIGameEndViewScript::UIGameEndViewScript() {
}

UIGameEndViewScript::~UIGameEndViewScript() {
}

bool UIGameEndViewScript::init() {
    ETNode<ETUIViewAppearAnimationEvents>::connect(getEntityId());
    ETNode<ETUIStarsAppearAnimationEvents>::connect(getEntityId());
    return true;
}

void UIGameEndViewScript::deinit() {
}

void UIGameEndViewScript::ET_onViewAppeared(EntityId viewId) {
    if(viewId != getEntityId()) {
        return;
    }

    EndGameResult gameResult;
    ET_SendEventReturn(gameResult, &ETGameEndResult::ET_getLastGameResult);

    // auto starsCount = covertToStarsCount(gameResult.objectiveCompleted);
    // if(starsCount > 0) {
    //     ET_SendEvent(&ETUIStarsAppearAnimation::ET_showStars, 3);
    // }

    ET_SendEvent(&ETUIStarsAppearAnimation::ET_showStars, 3);
}

void UIGameEndViewScript::ET_onStarsAppeared() {
}
