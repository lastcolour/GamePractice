#ifndef __ET_GAME_BOARD_HPP__
#define __ET_GAME_BOARD_HPP__

#include "Core/Core.hpp"
#include "Render/Color.hpp"
#include "Math/Vector.hpp"
#include "Math/AABB.hpp"

class ElementDescriptor;

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

struct ETGameBoardElemDestoryEvents {
    virtual ~ETGameBoardElemDestoryEvents() = default;
    virtual void ET_onElemsDestroyed(EntityId elemId) = 0;
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

struct ETGameBoardEvents {
    virtual ~ETGameBoardEvents() = default;
    virtual void ET_onAllElemsStatic() = 0;
};

struct ETGameBoardElemsPool {
    virtual ~ETGameBoardElemsPool() = default;
    virtual EntityId ET_spawnElem() = 0;
    virtual void ET_removeElem(EntityId elemId) = 0;
    virtual void ET_addElemsToSpawn(const ElementDescriptor& newElemDescr) = 0;
};

#endif /* __ET_GAME_BOARD_HPP__ */