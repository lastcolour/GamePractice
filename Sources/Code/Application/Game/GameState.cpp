#include "Game/GameState.hpp"
#include "UI/ETUIInterfaces.hpp"

#include <cassert>

namespace {

const char* GAME_END_EVENT = "Game_OnGameEnd";

}; // namespace

GameState::GameState() :
    gameState(EGameState::None) {
}

GameState::~GameState() {
}

bool GameState::init() {
    ETNode<ETGameState>::connect(getEntityId());
    ET_SendEvent(&ETGameTimer::ET_pauseTimer);
    return true;
}

void GameState::deinit() {
    ETNode<ETGameState>::disconnect();
}

void GameState::ET_startGame() {
    endResult.reset();
    gameState = EGameState::Running;
    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
}

void GameState::ET_pauseGame() {
    gameState = EGameState::Paused;
    ET_SendEvent(&ETGameTimer::ET_pauseTimer);
}

void GameState::ET_resumeGame() {
    gameState = EGameState::Running;
    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
}

void GameState::setupEndResult() {
    int gameScore = 0;
    ET_SendEventReturn(gameScore, &ETGameScore::ET_getGameScore);
    int highScore = 0;
    ET_SendEventReturn(highScore, &ETGameConfig::ET_getHighScore);
    bool newHighScore = false;
    if(gameScore > highScore) {
        ET_SendEvent(&ETGameConfig::ET_setHighScore, gameScore);
        newHighScore = true;
    }
    assert(!endResult && "Double game end");
    endResult.reset(new EndGameResult);
    endResult->score = gameScore;
    endResult->newHighScore = newHighScore;
}

void GameState::ET_endGame(EEndGameReason endReason) {
    switch (endReason)
    {
    case EEndGameReason::TimeOut: {
        setupEndResult();
        gameState = EGameState::None;
        ET_SendEvent(&ETUIEventManager::ET_onEvent, GAME_END_EVENT);
        break;
    }
    case EEndGameReason::Interrupt: {
        gameState = EGameState::None;
        break;
    }
    default:
        break;
    }
}

EGameState GameState::ET_getGameState() const {
    return gameState;
}

const EndGameResult* GameState::ET_getGameEndResult() const {
    if(endResult) {
        return endResult.get();
    }
    return nullptr;
}