#include "Game/GameStateManager.hpp"
#include "UI/ETUIViewScript.hpp"
#include "Game/ETGameBoard.hpp"

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
    ET_SendEvent(&ETGameTimer::ET_resume);
    ET_changeState(EGameState::PreGame);
}

void GameStateManager::ET_pauseGame() {
    ET_SendEvent(&ETGameTimer::ET_pause);
    if(gameState == EGameState::InGame) {
        ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, false);
    }
}

void GameStateManager::ET_resumeGame() {
    ET_SendEvent(&ETGameTimer::ET_resume);
    if(gameState == EGameState::InGame) {
        ET_SendEvent(&ETGameBoardInteractionLogic::ET_allowInteraction, true);
    }
}

bool GameStateManager::ET_isGamePaused() const {
    bool res = false;
    ET_SendEventReturn(res, &ETGameTimer::ET_isPaused);
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
    ET_SendEvent(&ETGameTimer::ET_pause);
}

void GameStateManager::ET_changeState(EGameState newState) {
    if(gameState == newState) {
        assert(false && "Double state change request");
        return;
    }
    auto prevState = gameState;
    gameState = newState;
    switch(gameState) {
        case EGameState::PreGame: {
            assert(prevState == EGameState::None && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: PreGame");
            preGame.onEnter(getEntityId());
            break;
        }
        case EGameState::InGame: {
            assert(prevState == EGameState::PreGame && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: InGame");
            preGame.onLeave();
            inGame.onEnter(getEntityId());
            break;
        }
        case EGameState::PostGame: {
            assert(prevState == EGameState::InGame && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: PostGame");
            inGame.onLeave();
            postGame.onEnter(getEntityId());
            break;
        }
        case EGameState::None: {
            assert(prevState == EGameState::PostGame && "Invalid new game state");
            LogInfo("[GameStateManager::ET_changeState] Change state to: None");
            ET_SendEvent(&ETGameTimer::ET_pause);
            postGame.onLeave();
            break;
        }
        default: {
            assert(false && "Invalid game state");
        }
    }
}