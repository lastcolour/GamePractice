#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <vector>
#include <memory>

#include "Game/GameObject.hpp"

class Game {

    typedef std::unique_ptr<GameObject> GameObjectPtrT;

public:

    Game();
    virtual ~Game();

    virtual bool init();
    virtual bool shouldRun();
    virtual void update();

    std::unique_ptr<GameObject> createObject(const std::string& objectName);

private:

    std::string createNewNameForObject(const std::string& baseName);

private:

    size_t objCount;
    std::vector<GameObjectPtrT> gameObjects;
};

#endif /* __GAME_HPP__ */