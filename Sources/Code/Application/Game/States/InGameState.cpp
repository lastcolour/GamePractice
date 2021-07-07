#include "Game/States/InGameState.hpp"

InGameState::InGameState() :
    gameTime(0.f) {
}

InGameState::~InGameState() {
}

void InGameState::onEnter(EntityId gameId) {
    gameEntityId = gameId;
    gameTime = 0.f;

    ET_SendEvent(&ETGameStateEvents::ET_onGameEnterState, EGameState::InGame);

    ETNode<ETGameLimitsEvents>::connect(gameEntityId);
    ETNode<ETGameObjectiveEvents>::connect(gameEntityId);
    ETNode<ETGameTimerEvents>::connect(gameEntityId);

    ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, true);
}

void InGameState::onLeave() {
    gameEntityId = InvalidEntityId;
    ET_SendEvent(&ETGameStateEvents::ET_onGameLeaveState, EGameState::InGame);

    ETNode<ETGameObjectiveEvents>::disconnect();
    ETNode<ETGameLimitsEvents>::disconnect();
    ETNode<ETGameTimerEvents>::disconnect();
}

void InGameState::ET_onGameTick(float dt) {
    gameTime += dt;
}

void InGameState::ET_onGameLimitReached() {
    ET_SendEvent(&ETGameScore::ET_setGameTime, gameTime);
    ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, false);

    bool hasActiveSwitching = false;
    ET_SendEventReturn(hasActiveSwitching, &ETGameBoardInteractionLogic::ET_hasActiveSwitching);
    if(hasActiveSwitching) {
        ETNode<ETGameBoardEvents>::connect(gameEntityId);
    } else {
        ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::PostGame);
    }
}

void InGameState::ET_onObjectiveCompleted(ObjectiveProgress type) {
    if(type == ObjectiveProgress::ThreeStars) {
        ET_onGameLimitReached();
    }
}

void InGameState::ET_onAllElemsStatic() {
    ETNode<ETGameBoardEvents>::disconnect();
    ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::PostGame);
}