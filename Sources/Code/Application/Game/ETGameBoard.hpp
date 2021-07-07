#ifndef __ET_GAME_BOARD_HPP__
#define __ET_GAME_BOARD_HPP__

#include "Render/Color.hpp"
#include "Math/AABB.hpp"

class BoardMatchState;
enum class EBoardElemType;

struct ETGameBoard {
    virtual ~ETGameBoard() = default;
    virtual void ET_spawnElems() = 0;
    virtual void ET_replaceElemToSpecial(EntityId targetId, EBoardElemType elemType) = 0;
    virtual void ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) = 0;
    virtual void ET_readBoardMatchState(BoardMatchState& boardMatchState) const = 0;
    virtual EntityId ET_getElemByPos(const Vec2i& pt) const = 0;
    virtual EntityId ET_getElemByBoardPos(const Vec2i& boardPt) const = 0;
    virtual Vec2i ET_getElemBoardPos(EntityId elemEntId) const = 0;
    virtual const Vec2i& ET_getBoardSize() const = 0;
    virtual const AABB2Di& ET_getBoardBox() const = 0;
    virtual int ET_getCellSize() const = 0;
    virtual Vec3 ET_getPosFromBoardPos(const Vec2i& boardPt) const = 0;
    virtual void ET_setUIElement(EntityId rootUIElementId) = 0;
    virtual bool ET_isAllElemStatic() const = 0;
    virtual void ET_setBlockElemMatching(bool flag) = 0;
    virtual void ET_resize(const AABB2D& newAabb) = 0;
    virtual std::vector<EntityId> ET_getAllElemsOfType(EBoardElemType queryElemType) const = 0;
};

struct ETGameBoardElemDestoryEvents {
    virtual ~ETGameBoardElemDestoryEvents() = default;
    virtual void ET_onElemsDestroyed(EntityId elemId) = 0;
};

struct ETGameBoardInteractionLogic {
    virtual ~ETGameBoardInteractionLogic() = default;
    virtual void ET_allowInteraction(bool flag) = 0;
    virtual bool ET_canInteract() const = 0;
    virtual bool ET_hasActiveSwitching() const = 0;
};

struct ETGameBoardInteractionEvents {
    virtual ~ETGameBoardInteractionEvents() = default;
    virtual void ET_onElemMoved() = 0;
};

struct ETGameBoardMatcher {
    virtual ~ETGameBoardMatcher() = default;
    virtual bool ET_matchElements() = 0;
    virtual void ET_playDestroyEffect(EntityId elemId) = 0;
};

struct ETGameBoardEvents {
    virtual ~ETGameBoardEvents() = default;
    virtual void ET_onAllElemsStatic() = 0;
};

struct ETGameBoardAnimation {
    virtual ~ETGameBoardAnimation() = default;
    virtual void ET_zoomOut() = 0;
};

struct ETGameBoardAnimationEvents {
    virtual ~ETGameBoardAnimationEvents() = default;
    virtual void ET_onZoomOutPlayed() = 0;
};

struct ETGameBoardElemHighlighter {
    virtual ~ETGameBoardElemHighlighter() = default;
    virtual void ET_highlightCell(const Vec2i& cellPt) = 0;
};

struct ETGameBoardSpawnerEvents {
    virtual ~ETGameBoardSpawnerEvents() = default;
    virtual void ET_onStartLoading() = 0;
};

#endif /* __ET_GAME_BOARD_HPP__ */