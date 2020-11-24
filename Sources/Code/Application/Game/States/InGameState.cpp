#include "Game/States/InGameState.hpp"
#include "Game/ETGameBoard.hpp"

InGameState::InGameState() :
    gameTime(0.f) {
}

InGameState::~InGameState() {
}

void InGameState::onEnter(EntityId gameEntityId) {
    gameTime = 0.f;

    ET_SendEvent(&ETGameStateEvents::ET_onGameEnterState, EGameState::InGame);

    ETNode<ETGameEndTimerEvents>::connect(gameEntityId);
    ETNode<ETGameObjectiveEvents>::connect(gameEntityId);
    ETNode<ETGameTimerEvents>::connect(gameEntityId);

    ET_SendEvent(&ETGameEndTimer::ET_startEndTimer);
    ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, true);
}

void InGameState::onLeave() {
    ET_SendEvent(&ETGameStateEvents::ET_onGameLeaveState, EGameState::InGame);

    ETNode<ETGameObjectiveEvents>::disconnect();
    ETNode<ETGameEndTimerEvents>::disconnect();
    ETNode<ETGameTimerEvents>::disconnect();
}

void InGameState::ET_onGameTick(float dt) {
    gameTime += dt;
}

void InGameState::ET_onGameTimeOut() {
    ET_SendEvent(&ETGameScore::ET_setGameTime, gameTime);
    ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, false);
    ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::PostGame);
}

void InGameState::ET_onObjectiveCompleted(ObjectiveProgress type) {
    if(type == ObjectiveProgress::ThreeStars) {
        ET_onGameTimeOut();
    }
}