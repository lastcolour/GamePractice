#ifndef __ET_GAME_INTERFACES_HPP__
#define __ET_GAME_INTERFACES_HPP__

#include "Render/Color.hpp"
#include "Core/ETPrimitives.hpp"
#include "Math/Vector.hpp"
#include "Math/AABB.hpp"

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
    virtual int ET_getGameScore() const = 0;
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

struct ETGameBoardElemDestoryEvents {
    virtual ~ETGameBoardElemDestoryEvents() = default;
    virtual void ET_onElemsDestroyed(int count) = 0;
};

enum class EBoardElemState {
    Void = 0,
    Static,
    Moving,
    Removing,
    Switching,
};

enum class EBoardElemType {
    None = 0,
    Red,
    Blue,
    Green,
    Purple,
    Yellow
};

struct ETGameBoard {
    virtual ~ETGameBoard() = default;
    virtual void ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) = 0;
    virtual void ET_setElemState(EntityId elemEntId, EBoardElemState newState) = 0;
    virtual EBoardElemState ET_getElemState(EntityId elemEntId) const = 0;
    virtual EBoardElemType ET_getElemType(EntityId elemEntId) const = 0;
    virtual void ET_updateBoard() = 0;
    virtual EntityId ET_getElemByPos(const Vec2i& pt) const = 0;
    virtual EntityId ET_getElemByBoardPos(const Vec2i& boardPt) const = 0;
    virtual Vec2i ET_getElemBoardPos(EntityId elemEntId) const = 0;
    virtual const Vec2i& ET_getBoardSize() const = 0;
    virtual const AABB2Di& ET_getBoardBox() const = 0;
    virtual int ET_getCellSize() const = 0;
    virtual Vec3 ET_getPosFromBoardPos(const Vec2i& boardPt) const = 0;
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

struct ETGameEndTimer {
    virtual ~ETGameEndTimer() = default;
    virtual void ET_startEndTimer() = 0;
    virtual float ET_getRemainingTime() const = 0;
    virtual float ET_getInitialEndGameDuration() const = 0;
};

struct ETGameEndTimerEvents {
    virtual ~ETGameEndTimerEvents() = default;
    virtual void ET_onGameTimeOut() = 0;
};

struct ETGameTimer {
    virtual ~ETGameTimer() = default;
    virtual void ET_pauseTimer() = 0;
    virtual void ET_resumeTimer() = 0;
    virtual bool ET_isTimerPaused() const = 0;
};

struct ETGameTimerEvents {
    virtual ~ETGameTimerEvents() = default;
    virtual void ET_onGameTick(float dt) = 0;
};

struct EndGameResult {
    int score;
    bool newHighScore;

    EndGameResult() :
        score(0), newHighScore(false) {}
};

enum class EGameState {
    PreGame = 0,
    InGame,
    PostGame,
    None
};

struct ETGameStateManager {
    virtual ~ETGameStateManager() = default;
    virtual void ET_initGame() = 0;
    virtual void ET_startGame() = 0;
    virtual void ET_pauseGame() = 0;
    virtual void ET_resumeGame() = 0;
    virtual bool ET_isGamePaused() const = 0;
    virtual bool ET_isGameState() const = 0;
    virtual void ET_interruptGame() = 0;
    virtual void ET_changeState(EGameState newGameState) = 0;
};

struct ETGameEndResult {
    virtual ~ETGameEndResult() = default;
    virtual const EndGameResult* ET_getGameEndResult() const = 0;
};

struct ETGameBoardAppearAnimation {
    virtual ~ETGameBoardAppearAnimation() = default;
    virtual void ET_startBoardAppearing() = 0;
};

struct ETGameBoardAppearAnimationEvents {
    virtual ~ETGameBoardAppearAnimationEvents() = default;
    virtual void ET_onBoardAppeared() = 0;
};

struct ETGAmeBoardInteractionLogic {
    virtual ~ETGAmeBoardInteractionLogic() = default;
    virtual void ET_allowInteraction(bool flag) = 0;
    virtual bool ET_canInteract() const = 0;
};

struct ETGameBoardMatcher {
    virtual ~ETGameBoardMatcher() = default;
    virtual std::vector<EntityId> ET_getMatchedElements() = 0;
};

#endif /* __ET_GAME_INTERFACES_HPP__ */