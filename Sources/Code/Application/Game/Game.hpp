#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "Core/SystemLogic.hpp"
#include "Game/GameObjectCreator.hpp"
#include "Game/GameETInterfaces.hpp"

#include <vector>
#include <memory>

class Game : public SystemLogic,
    public ETNode<ETGame> {

    typedef std::unique_ptr<GameObject> GameObjectPtrT;

public:

    Game();
    virtual ~Game();

    // ETGame
    EntityId ET_createGameObject(const std::string& objectName) override;

protected:

    // SystemLogic
    bool onInit() override;
    bool onShouldRun() override;
    void onUpdate() override;

private:

    GameObjectCreator gameObjCreator;
    std::vector<GameObjectPtrT> gameObjects;
};

#endif /* __GAME_HPP__ */