#ifndef __GAME_LOGIC_HPP__
#define __GAME_LOGIC_HPP__

#include "Game/GameObject.hpp"

class JSONNode;

class GameLogic {
public:

    virtual ~GameLogic();

    virtual bool init(const JSONNode& node) { return true; }

    void setGameObject(const GameObject* obj);
    EntityId getEntityId() const;

protected:

    GameLogic();

private:

    const GameObject* gameObj;
};

#endif /* __GAME_LOGIC_HPP__ */