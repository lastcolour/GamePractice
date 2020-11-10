#include "Game/ViewScripts/GameResultViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGameScore.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntity.hpp"

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
        classInfo->addField("levelName", &GameResultViewScript::levelNameId);
        classInfo->addField("continueButton", &GameResultViewScript::continueButtonId);
        classInfo->addField("timeValue", &GameResultViewScript::timeValueId);
        classInfo->addField("scoreValue", &GameResultViewScript::scoreValueId);
        classInfo->addField("effect", &GameResultViewScript::emitterId);
        classInfo->addField("stars", &GameResultViewScript::progressStars);
    }
}

GameResultViewScript::GameResultViewScript() :
    state(State::None) {
}

GameResultViewScript::~GameResultViewScript() {
}

bool GameResultViewScript::init() {
    BaseViewScript::init();
    ETNode<ETUIViewAppearAnimationEvents>::connect(getEntityId());
    return true;
}

void GameResultViewScript::onEvent(const UIEvent& event) {
    if(event.type == UIEvent::EventType::OnBackButton || event.type == UIEvent::EventType::OnGameEndViewExit) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::EndGame);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Levels);
    }
}

void GameResultViewScript::ET_onViewOpened() {
    BaseViewScript::ET_onViewOpened();
    state = State::None;
    ET_SendEvent(continueButtonId, &ETUIElement::ET_hide);

    EndGameResult gameResult;
    ET_SendEventReturn(gameResult, &ETGameEndResult::ET_getLastGameResult);

    auto scoreStr = StringFormat("%d", gameResult.score);
    ET_SendEvent(scoreValueId, &ETUILabel::ET_setText, scoreStr.c_str());

    auto timeStr = StringFormat("%.1f", gameResult.gameTime);
    ET_SendEvent(timeValueId, &ETUILabel::ET_setText, timeStr.c_str());

    ET_SendEvent(continueButtonId, &ETUIElement::ET_hide);
    ET_SendEvent(progressStars.fristId, &ETUIElement::ET_hide);
    ET_SendEvent(progressStars.secondId, &ETUIElement::ET_hide);
    ET_SendEvent(progressStars.thirdId, &ETUIElement::ET_hide);
}

void GameResultViewScript::ET_onAppeared(EntityId viewId) {
    if(viewId != waitingId) {
        return;
    }
    waitingId = InvalidEntityId;
    if(state == State::ShowingStars) {
        state = State::ShowingContinueButton;
        waitingId = continueButtonId;
        ET_SendEvent(continueButtonId, &ETUIElement::ET_disable);
        playAppearAnimation(continueButtonId);
    } else if(state == State::ShowingContinueButton) {
        ET_SendEvent(continueButtonId, &ETUIElement::ET_enable);
        state = State::Waiting;
    }
}

void GameResultViewScript::ET_onDisappeared(EntityId viewId) {
}

void GameResultViewScript::ET_onViewClosed() {
    BaseViewScript::ET_onViewClosed();
}

void GameResultViewScript::ET_onViewGetFocus() {
    BaseViewScript::ET_onViewGetFocus();

    state = State::ShowingStars;

    EndGameResult gameResult;
    ET_SendEventReturn(gameResult, &ETGameEndResult::ET_getLastGameResult);
    auto starsCount = covertToStarsCount(gameResult.objectiveCompleted);

    if(starsCount == 0) {
        state = State::ShowingContinueButton;
        ET_SendEvent(continueButtonId, &ETUIElement::ET_disable);
        playAppearAnimation(continueButtonId);
    } else {
            if(starsCount > 0) {
            playAppearAnimation(progressStars.fristId);
            waitingId = progressStars.fristId;
        }
        if(starsCount > 1) {
            playAppearAnimation(progressStars.secondId);
            waitingId = progressStars.secondId;
        }
        if(starsCount > 2) {
            playAppearAnimation(progressStars.thirdId);
            waitingId = progressStars.thirdId;
        }
    }
}

void GameResultViewScript::playAppearAnimation(EntityId elemId) {
    if(!elemId.isValid()) {
        return;
    }
    if(ET_IsExistNode<ETUIViewAppearAnimation>(elemId)) {
        ET_SendEvent(elemId, &ETUIViewAppearAnimation::ET_appear, getEntityId());
    } else {
        LogWarning("[GameResultViewScript::playAppearAnimation] Can't find required appear animation on entity: '%s'",
            EntityUtils::GetEntityName(elemId));
        ET_onAppeared(elemId);
    }
}