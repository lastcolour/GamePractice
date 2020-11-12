#ifndef __ET_GAME_HPP__
#define __ET_GAME_HPP__

enum class EGameState {
    PreGame = 0,
    InGame,
    PostGame,
    None
};

struct ETGameStateManager {
    virtual ~ETGameStateManager() = default;
    virtual void ET_startGame() = 0;
    virtual void ET_pauseGame() = 0;
    virtual void ET_resumeGame() = 0;
    virtual void ET_finishGame() = 0;
    virtual bool ET_isGamePaused() const = 0;
    virtual bool ET_isInGameState() const = 0;
    virtual void ET_changeState(EGameState newGameState) = 0;
};

struct ETGameStateEvents {
    virtual ~ETGameStateEvents() = default;
    virtual void ET_onGameEnterState(EGameState state) = 0;
    virtual void ET_onGameLeaveState(EGameState state) = 0;
};

#endif /* __ET_GAME_HPP__ */