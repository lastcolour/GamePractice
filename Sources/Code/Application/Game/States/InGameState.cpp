#include "Game/States/InGameState.hpp"
#include "Game/ETGameInterfaces.hpp"

InGameState::InGameState() {
}

InGameState::~InGameState() {
}

void InGameState::onEnter(EntityId gameEntityId) {
    ET_SendEvent(&ETGameStateEvents::ET_onGameEnterState, EGameState::InGame);

    ETNode<ETGameEndTimerEvents>::connect(gameEntityId);
    ETNode<ETGameObjectiveEvents>::connect(gameEntityId);

    ET_SendEvent(&ETGameEndTimer::ET_startEndTimer);
    ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, true);
}

void InGameState::onLeave() {
    ET_SendEvent(&ETGameStateEvents::ET_onGameLeaveState, EGameState::InGame);

    ETNode<ETGameObjectiveEvents>::disconnect();
    ETNode<ETGameEndTimerEvents>::disconnect();
}

void InGameState::ET_onGameTimeOut() {
    ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, false);
    ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::PostGame);
}

void InGameState::ET_onObjectiveCompleted(ObjectiveProgress type) {
    if(type == ObjectiveProgress::ThreeStars) {
        ET_onGameTimeOut();
    }
}