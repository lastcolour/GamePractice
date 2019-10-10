#include "Game/States/PreGameState.hpp"
#include "Game/ETGameInterfaces.hpp"

PreGameState::PreGameState() {
}

PreGameState::~PreGameState() {
}

void PreGameState::connect(EntityId entityId) {
    ETNode<ETGameBoardAppearAnimationEvents>::connect(entityId);
}

void PreGameState::onEnter() {
    ET_SendEvent(&ETGameBoardAppearAnimation::ET_startBoardAppearing);
}

void PreGameState::onLeave() {
}

void PreGameState::ET_onBoardAppeared() {
    ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::InGame);
}