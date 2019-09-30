#include "Game/GameStateManager.hpp"
#include "ETApplicationInterfaces.hpp"

#include <cassert>

GameStateManager::GameStateManager() :
    gameState(EGameState::None) {
}

GameStateManager::~GameStateManager() {
}

bool GameStateManager::init() {
    ETNode<ETGameStateManager>::connect(getEntityId());
    preGame.connect(getEntityId());
    inGame.connect(getEntityId());
    postGame.connect(getEntityId());
    return true;
}

void GameStateManager::deinit() {
    ETNode<ETGameStateManager>::disconnect();
}

void GameStateManager::ET_initGame() {
    ET_changeState(EGameState::PreGame);
}

void GameStateManager::ET_startGame() {
    assert(gameState == EGameState::PreGame && "Invalid game state");
    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
}

void GameStateManager::ET_pauseGame() {
    ET_SendEvent(&ETGameTimer::ET_pauseTimer);
}

void GameStateManager::ET_resumeGame() {
    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
}

bool GameStateManager::ET_isGamePaused() const {
    bool res = false;
    ET_SendEventReturn(res, &ETGameTimer::ET_isTimerPaused);
    return res;
}

bool GameStateManager::ET_isGameState() const {
    if(gameState == EGameState::None) {
        return false;
    }
    return true;
}

void GameStateManager::ET_interruptGame() {
    gameState = EGameState::None;
}

void GameStateManager::ET_changeState(EGameState newState) {
    if(gameState == newState) {
        assert(false && "Double state change request");
        return;
    }
    switch(newState) {
        case EGameState::PreGame: {
            assert(gameState == EGameState::None && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: PreGame");
            gameState = newState;
            preGame.onEnter();
            break;
        }
        case EGameState::InGame: {
            assert(gameState == EGameState::PreGame && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: InGame");
            gameState = newState;
            preGame.onLeave();
            inGame.onEnter();
            break;
        }
        case EGameState::PostGame: {
            assert(gameState == EGameState::InGame && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: PostGame");
            gameState = newState;
            inGame.onLeave();
            postGame.onEnter();
            break;
        }
        case EGameState::None: {
            assert(gameState == EGameState::PostGame && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: None");
            gameState = newState;
            postGame.onLeave();
            break;
        }
        default: {
            assert(false && "Invalid game state");
            return;
        }
    }
}