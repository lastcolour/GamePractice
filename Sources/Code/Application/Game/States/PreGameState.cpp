#include "Game/States/PreGameState.hpp"
#include "Game/ETGameInterfaces.hpp"

PreGameState::PreGameState() {
}

PreGameState::~PreGameState() {
}

void PreGameState::onEnter(EntityId gameEntityId) {
    ETNode<ETGameBoardAppearAnimationEvents>::connect(gameEntityId);
    ET_SendEvent(&ETGameScore::ET_resetScore);
    if(ET_IsExistNode<ETGameBoardAppearAnimation>()) {
        ET_SendEvent(&ETGameBoardAppearAnimation::ET_startBoardAppearing);
    } else {
        ET_onBoardAppeared();
    }
}

void PreGameState::onLeave() {
    ETNode<ETGameBoardAppearAnimationEvents>::disconnect();
}

void PreGameState::ET_onBoardAppeared() {
    ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::InGame);
}