#ifndef __ET_GAME_INTERFACES_HPP__
#define __ET_GAME_INTERFACES_HPP__

#include "Core/Core.hpp"
#include "Render/Color.hpp"
#include "Math/Vector.hpp"
#include "Math/AABB.hpp"

struct ETGameEndTimerUpdater {
    virtual ~ETGameEndTimerUpdater() = default;
    virtual void ET_setEndTime(float endTime) = 0;
};

struct ETGameScoreUpdater {
    virtual ~ETGameScoreUpdater() = default;
    virtual void ET_reset() = 0;
    virtual void ET_setGameScore(int score) = 0;
};

struct ETGameBoardElemDestoryEvents {
    virtual ~ETGameBoardElemDestoryEvents() = default;
    virtual void ET_onElemsDestroyed(EntityId elemId) = 0;
};

struct ETGameBoard {
    virtual ~ETGameBoard() = default;
    virtual void ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) = 0;
    virtual void ET_matchElements() = 0;
    virtual EntityId ET_getElemByPos(const Vec2i& pt) const = 0;
    virtual EntityId ET_getElemByBoardPos(const Vec2i& boardPt) const = 0;
    virtual Vec2i ET_getElemBoardPos(EntityId elemEntId) const = 0;
    virtual const Vec2i& ET_getBoardSize() const = 0;
    virtual const AABB2Di& ET_getBoardBox() const = 0;
    virtual int ET_getCellSize() const = 0;
    virtual Vec3 ET_getPosFromBoardPos(const Vec2i& boardPt) const = 0;
    virtual void ET_setUIElement(EntityId rootUIElementId) = 0;
    virtual bool ET_isAllElemStatic() const = 0;
};

struct ETGameBoardEvents {
    virtual ~ETGameBoardEvents() = default;
    virtual void ET_onAllElemsStatic() = 0;
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

struct ETGameBoardInteractionLogic {
    virtual ~ETGameBoardInteractionLogic() = default;
    virtual void ET_allowInteraction(bool flag) = 0;
    virtual bool ET_canInteract() const = 0;
};

struct ETGameBoardMatcher {
    virtual ~ETGameBoardMatcher() = default;
    virtual std::vector<EntityId> ET_getMatchedElements() = 0;
};

#endif /* __ET_GAME_INTERFACES_HPP__ */