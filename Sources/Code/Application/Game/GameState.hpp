#ifndef __GAME_STATE_HPP__
#define __GAME_STATE_HPP__

#include "Core/SystemLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameState : public SystemLogic,
    public ETNode<ETGameState> {
private:

    enum class EGameState {
        None,
        Paused,
        Running
    };

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
    bool ET_isGamePaused() const;
    void ET_endGame(EEndGameReason endReason) override;
   const EndGameResult* ET_getGameEndResult() const override;

private:

    void setupEndResult();

private:

    std::unique_ptr<EndGameResult> endResult;
    EGameState gameState;
};

#endif /* __GAME_STATE_HPP__ */