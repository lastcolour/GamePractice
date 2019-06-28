#ifndef __GAME_OBJECT_MANAGER_HPP__
#define __GAME_OBJECT_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

class GameObject;
class GameLogic;

class GameObjectManager : public SystemLogic,
    public ETNode<ETGameObjectManager> {

    using GameObjectPtrT = std::unique_ptr<GameObject>;
    typedef std::unique_ptr<GameLogic>(*LogicCreateFunc)();

public:

    GameObjectManager();
    virtual ~GameObjectManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETGameObjectManager
    EntityId ET_createGameObject(const std::string& objectName) override;
    void ET_destroyObject(EntityId entId) override;

private:

    std::unique_ptr<GameObject> createObject(const std::string& objectName);

    template<typename T>
    void registerLogic(const std::string& logicName) {
        auto createFunc = [](){
            return std::unique_ptr<GameLogic>(new T);
        };
        registerCreateLogic(logicName, createFunc);
    }

    void registerCreateLogic(const std::string& logicName, LogicCreateFunc createFunc);
    std::unique_ptr<GameLogic> createLogic(const std::string& logicName);

private:

    std::unordered_map<std::string, LogicCreateFunc> logics;
    std::vector<GameObjectPtrT> gameObjects;
};

#endif /* __GAME_OBJECT_MANAGER_HPP__ */