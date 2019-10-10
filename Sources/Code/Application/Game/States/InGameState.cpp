#include "Game/States/InGameState.hpp"
#include "Game/ETGameInterfaces.hpp"

InGameState::InGameState() {
}

InGameState::~InGameState() {
}

void InGameState::onEnter(EntityId gameEntityId) {
    ETNode<ETGameEndTimerEvents>::connect(gameEntityId);

    ET_SendEvent(&ETGameEndTimer::ET_startEndTimer);
    ET_SendEvent(&ETGAmeBoardInteractionLogic::ET_allowInteraction, true);
}

void InGameState::onLeave() {
    ETNode<ETGameEndTimerEvents>::disconnect();
}

void InGameState::ET_onGameTimeOut() {
    ET_SendEvent(&ETGAmeBoardInteractionLogic::ET_allowInteraction, false);
    ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::PostGame);
}
