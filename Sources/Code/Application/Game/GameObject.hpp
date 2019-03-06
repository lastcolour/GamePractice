#ifndef __GAME_OBJECT_HPP__
#define __GAME_OBJECT_HPP__

#include "Game/GameETInterfaces.hpp"

#include <string>
#include <memory>
#include <vector>

class GameLogic;

class GameObject :
    public ETNode<ETGameObject> {

    typedef std::unique_ptr<GameLogic> GameLogicPtr;

public:

    GameObject(const std::string& objectName, int entId);
    ~GameObject();

    void addLogic(GameLogicPtr&& logic);

    EntityId getEntityId() const { return entityId; }

    // ETGameObject
    const std::string& ET_getName() const override { return name; }

private:

    std::string name;
    std::vector<GameLogicPtr> logics;
    EntityId entityId;
};

#endif /* __GAME_OBJECT_HPP__ */