#ifndef __GAME_OBJECT_HPP__
#define __GAME_OBJECT_HPP__

#include "Game/ETGameInterfaces.hpp"

#include <string>
#include <memory>
#include <vector>

class GameLogic;

class GameObject :
    public ETNode<ETGameObject> {

    typedef std::unique_ptr<GameLogic> GameLogicPtr;

public:

    GameObject(const std::string& objectName, EntityId entId);
    ~GameObject();

    void addLogic(GameLogicPtr&& logic);

    EntityId getEntityId() const { return entityId; }

    // ETGameObject
    const std::string& ET_getName() const override;
    void ET_setParent(EntityId entId) override;
    void ET_addChild(EntityId entId) override;
    void ET_removeChild(EntityId entId) override;
    EntityId ET_getParentId() const override;
    const Transform& ET_getTransform() const override;
    void ET_setTransform(const Transform& transform) override;

private:

    Transform tm;
    std::vector<GameLogicPtr> logics;
    std::vector<EntityId> children;
    std::string name;
    EntityId parentId;
    EntityId entityId;
};

#endif /* __GAME_OBJECT_HPP__ */