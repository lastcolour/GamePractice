#include "Game/ViewScripts/GameResultViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGameScore.hpp"
#include "UI/ETUIGameEndView.hpp"
#include "UI/ETUIView.hpp"

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

void GameResultViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameResultViewScript>("GameResultViewScript")) {
    }
}

GameResultViewScript::GameResultViewScript() {
}

GameResultViewScript::~GameResultViewScript() {
}

void GameResultViewScript::onEvent(UIEventType eventType) {
    if(eventType == UIEventType::OnBackButton || eventType == UIEventType::OnGameEndViewExit) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::EndGame);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
    }
}

void GameResultViewScript::ET_onGetFocus() {
    BaseViewScript::ET_onGetFocus();

    EndGameResult gameResult;
    ET_SendEventReturn(gameResult, &ETGameEndResult::ET_getLastGameResult);

    auto starsCount = covertToStarsCount(gameResult.objectiveCompleted);
    if(starsCount > 0) {
        ET_SendEvent(&ETUIStarsAppearAnimation::ET_showStars, 3);
    }

    ET_SendEvent(&ETUIStarsAppearAnimation::ET_showStars, 3);
}