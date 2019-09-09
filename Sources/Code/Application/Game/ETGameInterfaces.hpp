#ifndef __ET_GAME_INTERFACES_HPP__
#define __ET_GAME_INTERFACES_HPP__

#include "Render/Color.hpp"
#include "Core/ETPrimitives.hpp"
#include "Math/Vector.hpp"

struct ETGameTick {
    virtual ~ETGameTick() = default;
    virtual void ET_onGameTick(float dt) = 0;
};

struct ETGameEndTimerUpdater {
    virtual ~ETGameEndTimerUpdater() = default;
    virtual void ET_setEndTime(float endTime) = 0; 
};

struct ETGameScore {
    virtual ~ETGameScore() = default;
    virtual void ET_onElemsDestroyed(int count) = 0;
};

struct ETGameScoreUpdater {
    virtual ~ETGameScoreUpdater() = default;
    virtual void ET_setGameScore(int score) = 0;
};

struct ETGameBoardElemSwitcher {
    virtual ~ETGameBoardElemSwitcher() = default;
    virtual void ET_switchBoardElems(EntityId firstId, EntityId secondId) = 0;
};

struct ETGameBoardElemDestroy {
    virtual ~ETGameBoardElemDestroy() = default;
    virtual void ET_destroyBoardElem(EntityId elemId) = 0;
};

enum class EBoardElemState {
    Static = 0,
    Moving,
    Removing,
    Switching,
    Void
};

struct ETGameBoard {
    virtual ~ETGameBoard() = default;
    virtual void ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) = 0;
    virtual void ET_setElemState(EntityId elemEntId, EBoardElemState newState) = 0;
    virtual EBoardElemState ET_getElemState(EntityId elemEntId) const = 0;
    virtual void ET_updateBoard() = 0;
    virtual EntityId ET_getElemByPos(const Vec2i& pt) const = 0;
    virtual EntityId ET_getElemByBoardPos(const Vec2i& boardPt) const = 0;
    virtual Vec2i ET_getElemBoardPos(EntityId elemEntId) const = 0;
    virtual int ET_getCellSize() const = 0;
};

struct ETGameConfig {
    virtual ~ETGameConfig() = default;
    virtual bool ET_isSoundEnabled() const = 0;
    virtual void ET_setSoundEnabled(bool flag) = 0;
    virtual bool ET_isVibrationEnabled() const = 0;
    virtual void ET_setVibrationEnabled(bool flag) = 0;
    virtual const char* ET_getMainViewName() const = 0;
    virtual int ET_getHighScore() const = 0;
    virtual void ET_setHighScore(int newHighScore) = 0;
};

struct ETGameEndTimer {
    virtual ~ETGameEndTimer() = default;
    virtual float ET_getRemainingTime() const = 0;
    virtual float ET_getInitialEndGameDuration() const = 0;
};

struct ETGameTimer {
    virtual ~ETGameTimer() = default;
    virtual void ET_pauseTimer() = 0;
    virtual void ET_resumeTimer() = 0;
};

struct ETGameTimerEvents {
    virtual ~ETGameTimerEvents() = default;
    virtual void ET_onGameTick(float dt) = 0;
};

enum class EGameState {
    None,
    Paused,
    Running
};

struct ETGameState {
    virtual ~ETGameState() = default;
    virtual void ET_startGame() = 0;
    virtual void ET_pauseGame() = 0;
    virtual void ET_resumeGame() = 0;
    virtual void ET_endGame() = 0;
    virtual void ET_interruptGame() = 0;
    virtual EGameState ET_getGameState() const = 0;
};

#endif /* __ET_GAME_INTERFACES_HPP__ */