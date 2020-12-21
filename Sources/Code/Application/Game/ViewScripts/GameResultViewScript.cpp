#include "Game/ViewScripts/GameResultViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGameScore.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntity.hpp"
#include "Game/GameUtils.hpp"
#include "Game/ETLevelProgress.hpp"

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

void GameResultViewScript::init() {
    BaseViewScript::init();
    ETNode<ETUIAnimationSequenceEvent>::connect(getEntityId());
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

void GameResultViewScript::ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) {
    if(waitingId != sourceId) {
        return;
    }
    if(animType != EAnimSequenceType::Appear) {
        return;
    }
    waitingId = InvalidEntityId;
    if(state == State::ShowingStars) {
        state = State::ShowingContinueButton;
        ET_SendEvent(continueButtonId, &ETUIElement::ET_disable);
        playAppearAnimation(continueButtonId);
    } else if(state == State::ShowingContinueButton) {
        ET_SendEvent(continueButtonId, &ETUIElement::ET_enable);
        state = State::Waiting;
    }
}

void GameResultViewScript::ET_onViewClosed() {
    BaseViewScript::ET_onViewClosed();
}

void GameResultViewScript::ET_onViewGetFocus() {
    BaseViewScript::ET_onViewGetFocus();

    state = State::ShowingStars;

    EndGameResult gameResult;
    ET_SendEventReturn(gameResult, &ETGameEndResult::ET_getLastGameResult);
    auto starsCount = GameUtils::ConvertToStarsCount(gameResult.objectiveCompleted);

    if(starsCount == 0) {
        state = State::ShowingContinueButton;
        ET_SendEvent(continueButtonId, &ETUIElement::ET_disable);
        playAppearAnimation(continueButtonId);
    } else {
        if(starsCount > 0) {
            playAppearAnimation(progressStars.fristId);
        }
        if(starsCount > 1) {
            playAppearAnimation(progressStars.secondId);
        }
        if(starsCount > 2) {
            playAppearAnimation(progressStars.thirdId);
        }
    }
}

void GameResultViewScript::playAppearAnimation(EntityId elemId) {
    if(!elemId.isValid()) {
        return;
    }
    waitingId = elemId;
    bool animStarted = UI::PlayAnimation(elemId, EAnimSequenceType::Appear, getEntityId());
    if(!animStarted) {
        LogWarning("[GameResultViewScript::playAppearAnimation] Can't find appear animation to play on entity: '%s'",
            EntityUtils::GetEntityName(elemId));
        ET_SendEvent(elemId, &ETUIElement::ET_show);
        ET_onAnimationPlayed(elemId, EAnimSequenceType::Appear);
    }
}