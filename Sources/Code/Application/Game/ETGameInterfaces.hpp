#ifndef __ET_GAME_INTERFACES_HPP__
#define __ET_GAME_INTERFACES_HPP__

#include "Render/Color.hpp"
#include "Math/Transform.hpp"
#include "Core/ETPrimitives.hpp"

#include <string>

struct ETGameBoardElemLogic {
    virtual ~ETGameBoardElemLogic() = default;
    virtual void ET_setBoardPos(const Vec2i& pt) = 0;
    virtual const Vec2i& ET_getBoardPos() const = 0;
};

struct ETGameObject {
    virtual ~ETGameObject() = default;
    virtual void ET_setParent(EntityId entId) = 0;
    virtual void ET_addChild(EntityId entId) = 0;
    virtual void ET_removeChild(EntityId entId) = 0;
    virtual EntityId ET_getParentId() const = 0;
    virtual const std::string& ET_getName() const = 0;
    virtual const Transform& ET_getTransform() const = 0;
    virtual void ET_setTransform(const Transform& tm) = 0;
};

struct ETGameTick {
    virtual ~ETGameTick() = default;
    virtual void ET_onGameTick(float dt) = 0;
};

struct ETGame {
    virtual ~ETGame() = default;
    virtual EntityId ET_createGameObject(const std::string& objectName) = 0;
    virtual void ET_destroyObject(EntityId entId) = 0;
};

#endif /* __ET_GAME_INTERFACES_HPP__ */