#include "Game/ViewScripts/GameResultViewScript.hpp"
#include "Game/ETGameScore.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Game/GameUtils.hpp"
#include "Game/ETLevelProgress.hpp"
#include "Render/ETParticlesSystem.hpp"

namespace {

void queueStarAppearEvent(EventSequence& eventSeq, EntityId startId, EntityId shakeBoxId, SoundEvent& soundEvent) {
    {
        EventSequence::Event event;
        event.targetId = startId;
        event.animType = EAnimSequenceType::Appear;
        event.onEndCallback = [&soundEvent](){
            soundEvent.emit();
        };
        eventSeq.addEvent(event);
    }
    {
        EventSequence::Event event;
        event.targetId = shakeBoxId;
        event.animType = EAnimSequenceType::Shake;
        eventSeq.addEvent(event);
    }
}

} // namespace

void GameResultViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameResultViewScript>("GameResultViewScript")) {
        classInfo->addField("levelName", &GameResultViewScript::levelNameId);
        classInfo->addField("continueButton", &GameResultViewScript::continueButtonId);
        classInfo->addField("timeValue", &GameResultViewScript::timeValueId);
        classInfo->addField("timeBox", &GameResultViewScript::timeBoxId);
        classInfo->addField("scoreValue", &GameResultViewScript::scoreValueId);
        classInfo->addField("scoreBox", &GameResultViewScript::scoreBoxId);
        classInfo->addField("shakeBox", &GameResultViewScript::shakeBoxId);
        classInfo->addField("effect", &GameResultViewScript::emitterId);
        classInfo->addField("starAppearSound", &GameResultViewScript::starAppearSound);
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

    eventSeq.forceFinish();

    ET_SendEvent(emitterId, &ETParticlesSystem::ET_removeAll);
}

void GameResultViewScript::ET_onViewGetFocus() {
    BaseViewScript::ET_onViewGetFocus();

    EndGameResult gameResult;
    ET_SendEventReturn(gameResult, &ETGameEndResult::ET_getLastGameResult);
    auto starsCount = GameUtils::ConvertToStarsCount(gameResult.objectiveCompleted);

    ET_SendEvent(emitterId, &ETParticlesSystem::ET_emit);

    if(starsCount > 0) {
        queueStarAppearEvent(eventSeq, progressStars.fristId, shakeBoxId, starAppearSound);
    }
    if(starsCount > 1) {
        queueStarAppearEvent(eventSeq, progressStars.secondId, shakeBoxId, starAppearSound);
    }
    if(starsCount > 2) {
        queueStarAppearEvent(eventSeq, progressStars.thirdId, shakeBoxId, starAppearSound);
    }
    {
        EventSequence::Event event;
        event.targetId = scoreBoxId;
        event.animType = EAnimSequenceType::Highlight;
        eventSeq.addEvent(event);
    }
    {
        EventSequence::Event event;
        event.targetId = timeBoxId;
        event.animType = EAnimSequenceType::Highlight;
        eventSeq.addEvent(event);
    }
    {
        EventSequence::Event event;
        event.targetId = continueButtonId;
        event.animType = EAnimSequenceType::Appear;
        eventSeq.addEvent(event);
    }
    eventSeq.start();
}

void GameResultViewScript::ET_onViewLostFocus() {
    BaseViewScript::ET_onViewLostFocus();

    ET_SendEvent(emitterId, &ETParticlesSystem::ET_stopEmitting);
}