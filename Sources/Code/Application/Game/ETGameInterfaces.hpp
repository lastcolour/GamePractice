#ifndef __ET_GAME_INTERFACES_HPP__
#define __ET_GAME_INTERFACES_HPP__

#include "Render/Color.hpp"
#include "Math/Transform.hpp"
#include "Core/ETPrimitives.hpp"

#include <string>

struct ETGameObjectEvents {
    virtual ~ETGameObjectEvents() = default;
    virtual void ET_onTransformChanged(const Transform& newTm) = 0;
    virtual void ET_onChildAdded(EntityId childId) = 0;
};

struct ETGameObject {
    virtual ~ETGameObject() = default;
    virtual const std::vector<EntityId>& ET_getChildren() const = 0;
    virtual void ET_setParent(EntityId entId) = 0;
    virtual void ET_addChild(EntityId entId) = 0;
    virtual void ET_removeChild(EntityId entId) = 0;
    virtual EntityId ET_getParentId() const = 0;
    virtual const char* ET_getName() const = 0;
    virtual const Transform& ET_getTransform() const = 0;
    virtual void ET_setTransform(const Transform& tm) = 0;
};

struct ETGameTick {
    virtual ~ETGameTick() = default;
    virtual void ET_onGameTick(float dt) = 0;
};

struct ETGameObjectManager {
    virtual ~ETGameObjectManager() = default;
    virtual EntityId ET_createGameObject(const char* objectName) = 0;
    virtual void ET_destroyObject(EntityId entId) = 0;
};

struct ETGameEndTimerUpdater {
    virtual ~ETGameEndTimerUpdater() = default;
    virtual void ET_setEndTime(float endTime) = 0; 
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

#endif /* __ET_GAME_INTERFACES_HPP__ */