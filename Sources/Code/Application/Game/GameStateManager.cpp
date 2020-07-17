#include "Game/GameStateManager.hpp"
#include "Core/ETLogger.hpp"
#include "UI/ETUIView.hpp"

#include <cassert>

GameStateManager::GameStateManager() :
    gameState(EGameState::None) {
}

GameStateManager::~GameStateManager() {
}

bool GameStateManager::init() {
    ETNode<ETGameStateManager>::connect(getEntityId());
    return true;
}

void GameStateManager::deinit() {
    ETNode<ETGameStateManager>::disconnect();
}

void GameStateManager::ET_startGame() {
    assert(gameState == EGameState::None && "Invalid game state");
    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
    ET_changeState(EGameState::PreGame);
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

bool GameStateManager::ET_isInGameState() const {
    if(gameState == EGameState::None) {
        return false;
    }
    return true;
}

void GameStateManager::ET_finishGame() {
    switch(gameState)
    {
    case EGameState::PreGame: {
        preGame.onLeave();
        break;
    }
    case EGameState::InGame: {
        inGame.onLeave();
        break;
    }
    case EGameState::PostGame: {
        postGame.onLeave();
        break;
    }
    case EGameState::None: {
        return;
    }
    default:
        assert(false && "Invalid game state");
        break;
    }
    gameState = EGameState::None;
    ET_SendEvent(&ETGameTimer::ET_pauseTimer);
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
            preGame.onEnter(getEntityId());
            break;
        }
        case EGameState::InGame: {
            assert(gameState == EGameState::PreGame && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: InGame");
            gameState = newState;
            preGame.onLeave();
            inGame.onEnter(getEntityId());
            break;
        }
        case EGameState::PostGame: {
            assert(gameState == EGameState::InGame && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: PostGame");
            gameState = newState;
            inGame.onLeave();
            postGame.onEnter(getEntityId());
            break;
        }
        case EGameState::None: {
            assert(gameState == EGameState::PostGame && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: None");
            gameState = newState;
            postGame.onLeave();
            ET_SendEvent(&ETUIEventManager::ET_onEvent, UIEventType::OnGameEnd);
            break;
        }
        default: {
            assert(false && "Invalid game state");
            return;
        }
    }
}