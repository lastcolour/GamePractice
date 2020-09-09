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

struct ETGameConfig {
    virtual ~ETGameConfig() = default;
    virtual bool ET_isSoundEnabled() const = 0;
    virtual void ET_setSoundEnabled(bool flag) = 0;
    virtual bool ET_isVibrationEnabled() const = 0;
    virtual void ET_setVibrationEnabled(bool flag) = 0;
    virtual int ET_getHighScore() const = 0;
    virtual void ET_setHighScore(int newHighScore) = 0;
};

#endif /* __ET_GAME_HPP__ */