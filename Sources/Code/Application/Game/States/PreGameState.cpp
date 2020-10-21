#include "Game/States/PreGameState.hpp"
#include "Game/ETGame.hpp"

PreGameState::PreGameState() {
}

PreGameState::~PreGameState() {
}

void PreGameState::onEnter(EntityId gameEntityId) {
    ET_SendEvent(&ETGameStateEvents::ET_onGameEnterState, EGameState::PreGame);
    ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::InGame);
}

void PreGameState::onLeave() {
    ET_SendEvent(&ETGameStateEvents::ET_onGameLeaveState, EGameState::PreGame);
}