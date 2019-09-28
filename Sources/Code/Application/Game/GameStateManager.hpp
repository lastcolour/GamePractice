#ifndef __GAME_STATE_TRANSITION_HPP__
#define __GAME_STATE_TRANSITION_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Game/States/PreGameState.hpp"
#include "Game/States/InGameState.hpp"
#include "Game/States/PostGameState.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameStateManager : public SystemLogic,
    public ETNode<ETGameStateManager> {
public:

    GameStateManager();
    virtual ~GameStateManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETGameStateManager
    void ET_initGame() override;
    void ET_startGame() override;
    void ET_pauseGame() override;
    void ET_resumeGame() override;
    bool ET_isGamePaused() const;
    bool ET_isGameState() const override;
    void ET_interruptGame() override;
    void ET_changeState(EGameState newState);

private:

    PreGameState preGame;
    InGameState inGame;
    PostGameState postGame;
    EGameState gameState;
};

#endif /* __GAME_STATE_TRANSITION_HPP__ */