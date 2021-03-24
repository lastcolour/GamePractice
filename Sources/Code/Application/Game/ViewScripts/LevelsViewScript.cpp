#include "Game/ViewScripts/LevelsViewScript.hpp"
#include "Game/ETGame.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Game/ETLevelProgress.hpp"
#include "Game/Progression/LevelsProgressData.hpp"
#include "Game/ETGameMusic.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUIAnimation.hpp"
#include "UI/ETUIScrollArea.hpp"

void LevelsViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelsViewScript>("LevelsViewScript")) {
        classInfo->addField("progressLabelId", &LevelsViewScript::progressLabelId);
        classInfo->addField("progressBoxId", &LevelsViewScript::progressBoxId);
        classInfo->addField("scrollAreaId", &LevelsViewScript::scrollAreaId);
    }
}

LevelsViewScript::LevelsViewScript() {
}

LevelsViewScript::~LevelsViewScript() {
}

void LevelsViewScript::ET_onViewClosed() {
    ET_SendEvent(&ETLevelsProgression::ET_resetProgressDelta);
}

void LevelsViewScript::ET_onViewOpened() {
    BaseViewScript::ET_onViewOpened();
    eventSeq.init(getEntityId());

    int totalStars = 0;
    ET_SendEventReturn(totalStars, &ETLevelButtonList::ET_getTotalStars);

    int doneStars = 0;
    ET_SendEventReturn(doneStars, &ETLevelButtonList::ET_getDoneStars);

    std::string progressStr;
    if(totalStars == doneStars) {
        progressStr = StringFormat("100%% (%d / %d)", totalStars, totalStars);
    } else {
        float fProg = (doneStars * 100.f) / static_cast<float>(totalStars);
        int prog = static_cast<int>(ceil(fProg));
        progressStr = StringFormat("%d%% (%d / %d)", prog, doneStars, totalStars);
    }

    ET_SendEvent(progressLabelId, &ETUILabel::ET_setText, progressStr.c_str());

    EntityId lastUnlockedBtId;
    ET_SendEventReturn(lastUnlockedBtId, &ETLevelButtonList::ET_getLastUnlockedLevelButton);
    ET_SendEvent(scrollAreaId, &ETUIScrollFocus::ET_setFocusToEntity, lastUnlockedBtId);
}

void LevelsViewScript::ET_onViewGetFocus() {
    BaseViewScript::ET_onViewGetFocus();

    ET_SendEvent(&ETGameMusic::ET_setTheme, EMusicTheme::Menu);

    ET_SendEvent(&ETLevelButtonList::ET_updateLevelProgress, eventSeq);

    const LevelProgressDelta* progressDelta = nullptr;
    ET_SendEventReturn(progressDelta, &ETLevelsProgression::ET_getProgressDelta);
    if(progressDelta) {
        auto starsDelta = progressDelta->current.stars - progressDelta->prev.stars;
        if(starsDelta > 0) {
            EventSequence::Event event;
            event.targetId = progressBoxId;
            event.animType = EAnimSequenceType::Highlight;
            eventSeq.addEvent(event);
        }
    }
    {
        EventSequence::Event event;
        event.onEndCallback = [this](){
            ET_SendEvent(scrollAreaId, &ETUIElement::ET_enable);
        };
        eventSeq.addEvent(event);
    }

    ET_SendEvent(scrollAreaId, &ETUIElement::ET_disable);
    eventSeq.start();
}

void LevelsViewScript::ET_onViewLostFocus() {
    eventSeq.forceFinish();
}

void LevelsViewScript::onEvent(const UIEvent& event) {
    if(event.type == UIEvent::EventType::OnBackButton) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Levels);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
    } else if(event.type == UIEvent::EventType::OnLevelsStartLevel) {
        std::string levelName;
        ET_SendEventReturn(levelName, &ETLevelButtonList::ET_getLevelNameForSender, event.senderId);
        if(levelName.empty()) {
            LogWarning("[LevelsViewScript::onEvent] Can't select level with empty name");
            return;
        }
        ET_QueueEvent(&ETGameBoardSpawner::ET_setPendingLevel, levelName);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Levels);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Game);
    }
}