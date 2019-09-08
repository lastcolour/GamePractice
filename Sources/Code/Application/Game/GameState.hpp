#ifndef __GAME_STATE_HPP__
#define __GAME_STATE_HPP__

#include "Core/SystemLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameState : public SystemLogic,
    public ETNode<ETGameState> {
public:
    GameState();
    virtual ~GameState();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETGameState
    void ET_startGame() override;
    void ET_pauseGame() override;
    void ET_resumeGame() override;
    void ET_endGame() override;
    EGameState ET_getGameState() const override;

private:

    EGameState gameState;
};

#endif /* __GAME_STATE_HPP__ */