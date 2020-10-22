#include "Game/ViewScripts/GameResultViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGameScore.hpp"
#include "UI/ETUIView.hpp"
#include "Render/ETRenderNode.hpp"

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
        classInfo->addField("stars", &GameResultViewScript::progressStars);
    }
}

GameResultViewScript::GameResultViewScript() {
}

GameResultViewScript::~GameResultViewScript() {
}

void GameResultViewScript::onEvent(const UIEvent& event) {
    if(event.type == UIEvent::EventType::OnBackButton || event.type == UIEvent::EventType::OnGameEndViewExit) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::EndGame);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Levels);
    }
}

void GameResultViewScript::ET_onGetFocus() {
    BaseViewScript::ET_onGetFocus();

    EndGameResult gameResult;
    ET_SendEventReturn(gameResult, &ETGameEndResult::ET_getLastGameResult);

    auto starsCount = covertToStarsCount(gameResult.objectiveCompleted);
    if(starsCount > 0) {
        ET_SendEvent(progressStars.fristId, &ETRenderNode::ET_show);
        ET_SendEvent(progressStars.fristId, &ETUIViewAppearAnimation::ET_appear, getEntityId());
    }
    if(starsCount > 1) {
        ET_SendEvent(progressStars.secondId, &ETRenderNode::ET_show);
        ET_SendEvent(progressStars.secondId, &ETUIViewAppearAnimation::ET_appear, getEntityId());
    }
    if(starsCount > 2) {
        ET_SendEvent(progressStars.thirdId, &ETRenderNode::ET_show);
        ET_SendEvent(progressStars.thirdId, &ETUIViewAppearAnimation::ET_appear, getEntityId());
    }
}