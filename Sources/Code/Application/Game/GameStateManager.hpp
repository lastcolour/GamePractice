#ifndef __GAME_STATE_MANAGER_HPP__
#define __GAME_STATE_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Game/States/PreGameState.hpp"
#include "Game/States/InGameState.hpp"
#include "Game/States/PostGameState.hpp"
#include "Game/ETGame.hpp"

class GameStateManager : public SystemLogic,
    public ETNode<ETGameStateManager> {
public:

    GameStateManager();
    virtual ~GameStateManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETGameStateManager
    void ET_startGame() override;
    void ET_pauseGame() override;
    void ET_resumeGame() override;
    void ET_finishGame() override;
    bool ET_isGamePaused() const override;
    bool ET_isInGameState() const override;
    void ET_changeState(EGameState newGameState) override;

private:

    PreGameState preGame;
    InGameState inGame;
    PostGameState postGame;
    EGameState gameState;
};

#endif /* __GAME_STATE_MANAGER_HPP__ */