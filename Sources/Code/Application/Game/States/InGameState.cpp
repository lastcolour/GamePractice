#include "Game/States/InGameState.hpp"
#include "Game/ETGameInterfaces.hpp"

InGameState::InGameState() {
}

InGameState::~InGameState() {
}

void InGameState::onEnter(EntityId gameEntityId) {
    ETNode<ETGameEndTimerEvents>::connect(gameEntityId);
    ETNode<ETGameObjectiveEvents>::connect(gameEntityId);

    ET_SendEvent(&ETGameEndTimer::ET_startEndTimer);
    ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, true);
}

void InGameState::onLeave() {
    ETNode<ETGameObjectiveEvents>::disconnect();
    ETNode<ETGameEndTimerEvents>::disconnect();
}

void InGameState::ET_onGameTimeOut() {
    ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, false);
    ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::PostGame);
}

void InGameState::ET_onObjectiveCompleted(ObjectiveType type) {
    if(type == ObjectiveType::ThreeStars) {
        ET_onGameTimeOut();
    }
}