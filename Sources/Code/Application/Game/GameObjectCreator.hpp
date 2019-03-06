#ifndef __GAME_OBJECT_CREATOR_HPP__
#define __GAME_OBJECT_CREATOR_HPP__

#include <memory>
#include <string>
#include <unordered_map>

class GameObject;
class GameLogic;

class GameObjectCreator {

    typedef std::unique_ptr<GameLogic>(*LogicCreateFunc)();

public:

    GameObjectCreator();
    ~GameObjectCreator();

    std::unique_ptr<GameObject> createObject(const std::string& objectName);

private:

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
};

#endif /* __GAME_OBJECT_CREATOR_HPP__ */