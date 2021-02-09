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

void GameResultViewScript::ET_onViewOpened() {
    eventSeq.init(getEntityId());

    BaseViewScript::ET_onViewOpened();

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

void GameResultViewScript::ET_onViewClosed() {
    BaseViewScript::ET_onViewClosed();
}

void GameResultViewScript::ET_onViewGetFocus() {
    BaseViewScript::ET_onViewGetFocus();

    EndGameResult gameResult;
    ET_SendEventReturn(gameResult, &ETGameEndResult::ET_getLastGameResult);
    auto starsCount = GameUtils::ConvertToStarsCount(gameResult.objectiveCompleted);

    ET_SendEvent(continueButtonId, &ETUIElement::ET_disable);

    if(starsCount > 0) {
        EventSequence::Event event;
        event.targetId = progressStars.fristId;
        event.animType = EAnimSequenceType::Appear;
        eventSeq.addEvent(event);
    }
    if(starsCount > 1) {
        EventSequence::Event event;
        event.targetId = progressStars.secondId;
        event.animType = EAnimSequenceType::Appear;
        eventSeq.addEvent(event);
    }
    if(starsCount > 2) {
        EventSequence::Event event;
        event.targetId = progressStars.thirdId;
        event.animType = EAnimSequenceType::Appear;
        eventSeq.addEvent(event);
    }
    {
        EventSequence::Event event;
        event.targetId = continueButtonId;
        event.animType = EAnimSequenceType::Appear;
        event.onEndCallback = [this](){
            ET_SendEvent(continueButtonId, &ETUIElement::ET_enable);
        };
        eventSeq.addEvent(event);
    }
    eventSeq.start();
}