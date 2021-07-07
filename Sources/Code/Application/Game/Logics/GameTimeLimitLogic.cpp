#include "Game/Logics/GameTimeLimitLogic.hpp"
#include "UI/ETUIBox.hpp"

void GameTimeLimitLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameTimeLimitLogic>("GameTimeLimitLogic")) {
        classInfo->addField("gameDuration", &GameTimeLimitLogic::gameDuration);
    }
}

GameTimeLimitLogic::GameTimeLimitLogic() : 
    gameDuration(10.f),
    currTime(0.f) {
}

GameTimeLimitLogic::~GameTimeLimitLogic() {
}

void GameTimeLimitLogic::init() {
    ETNode<ETGameLimits>::connect(getEntityId());
    ETNode<ETGameStateEvents>::connect(getEntityId());
}

void GameTimeLimitLogic::deinit() {
}

void GameTimeLimitLogic::ET_onGameTick(float dt) {
    currTime += dt;
    std::string text = StringFormat("%.1f", std::max(0.f, gameDuration - currTime));
    ET_SendEvent(labelId, &ETUILabel::ET_setText, text.c_str());
    if(currTime > gameDuration) {
        ET_SendEvent(&ETGameLimitsEvents::ET_onGameLimitReached);
    }
}

void GameTimeLimitLogic::ET_setLabelId(EntityId newLabelId) {
    labelId = newLabelId;
    if(!labelId.isValid()) {
        LogWarning("[GameTimeLimitLogic::ET_setLabelId] Invalid label id");
        return;
    }
    std::string text = StringFormat("%.1f", gameDuration);
    ET_SendEvent(labelId, &ETUILabel::ET_setText, text.c_str());
}

EGameLimitType GameTimeLimitLogic::ET_getLimitType() const {
    return EGameLimitType::Time;
}

void GameTimeLimitLogic::ET_onGameEnterState(EGameState state) {
    currTime = 0.f;
    if(state == EGameState::InGame) {
        ETNode<ETGameTimerEvents>::connect(getEntityId());
    }
}

void GameTimeLimitLogic::ET_onGameLeaveState(EGameState state) {
    if(state == EGameState::InGame) {
        ETNode<ETGameTimerEvents>::connect(getEntityId());
    }
}