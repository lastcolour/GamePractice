#ifndef __GAME_LOGIC_HPP__
#define __GAME_LOGIC_HPP__

#include "Core/Core.hpp"

class GameObject;
class JSONNode;

class GameLogic {
public:

    virtual ~GameLogic();

    virtual bool serialize(const JSONNode& node) = 0;
    virtual bool init() = 0;

    void setGameObject(const GameObject* obj);
    EntityId getEntityId() const;
    EntityId getParentId() const;

protected:

    GameLogic();

private:

    const GameObject* gameObj;
};

#endif /* __GAME_LOGIC_HPP__ */