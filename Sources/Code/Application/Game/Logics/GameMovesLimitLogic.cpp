#include "Game/Logics/GameMovesLimitLogic.hpp"
#include "UI/ETUIBox.hpp"

#include <cassert>

void GameMovesLimitLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameMovesLimitLogic>("GameMovesLimitLogic")) {
        classInfo->addField("maxMoves", &GameMovesLimitLogic::movesLimit);
    }
}

GameMovesLimitLogic::GameMovesLimitLogic() :
    movesLimit(10),
    currMoves(0) {
}

GameMovesLimitLogic::~GameMovesLimitLogic() {
}

void GameMovesLimitLogic::init() {
    ETNode<ETGameLimits>::connect(getEntityId());
    ETNode<ETGameStateEvents>::connect(getEntityId());
}

void GameMovesLimitLogic::deinit() {
}

void GameMovesLimitLogic::ET_setLabelId(EntityId newLabelId) {
    labelId = newLabelId;
    if(!labelId.isValid()) {
        LogWarning("[GameMovesLimitLogic::ET_setLabelId] Invalid label id");
        return;
    }
    std::string text = StringFormat("%d", movesLimit);
    ET_SendEvent(labelId, &ETUILabel::ET_setText, text.c_str());
}

EGameLimitType GameMovesLimitLogic::ET_getLimitType() const {
    return EGameLimitType::Moves;
}

void GameMovesLimitLogic::ET_onElemMoved() {
    currMoves += 1;
    std::string text = StringFormat("%d", std::max(0, movesLimit - currMoves));
    ET_SendEvent(labelId, &ETUILabel::ET_setText, text.c_str());
    if(currMoves >= movesLimit) {
        ET_SendEvent(&ETGameLimitsEvents::ET_onGameLimitReached);
    }
}

void GameMovesLimitLogic::ET_onGameEnterState(EGameState state) {
    currMoves = 0;
    if(state == EGameState::InGame) {
        ETNode<ETGameBoardInteractionEvents>::connect(getEntityId());
    }
}

void GameMovesLimitLogic::ET_onGameLeaveState(EGameState state) {
    if(state == EGameState::InGame) {
        ETNode<ETGameBoardInteractionEvents>::disconnect();
    }
}