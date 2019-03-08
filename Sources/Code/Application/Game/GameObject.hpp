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
    const std::string& ET_getName() const override;
    const Transform& ET_getTransform() const override;
    void ET_setTransform(const Transform& transform) override;

private:

    Transform tm;
    std::vector<GameLogicPtr> logics;
    std::string name;
    EntityId entityId;
};

#endif /* __GAME_OBJECT_HPP__ */