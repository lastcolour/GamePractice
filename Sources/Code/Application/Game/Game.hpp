#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/SystemModule.hpp"
#include "Game/GameObjectCreator.hpp"
#include "Game/ETGameInterfaces.hpp"

#include <vector>
#include <memory>

class Game : public SystemLogic,
    // public SystemModule,
    public ETNode<ETGame> {

    typedef std::unique_ptr<GameObject> GameObjectPtrT;

public:

    Game();
    virtual ~Game();

    void setRootObject(const std::string& rootObjectName);

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETGame
    EntityId ET_createGameObject(const std::string& objectName) override;
    void ET_destroyObject(EntityId entId) override;

private:

    std::string rootObject;
    GameObjectCreator gameObjCreator;
    std::vector<GameObjectPtrT> gameObjects;
};

#endif /* __GAME_HPP__ */