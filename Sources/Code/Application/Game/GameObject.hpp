#ifndef __GAME_OBJECT_HPP__
#define __GAME_OBJECT_HPP__

#include "Game/GameETInterfaces.hpp"

#include <string>
#include <memory>
#include <vector>

class BaseGameLogic;

class GameObject :
    public ETNode<ETGameObject> {

    typedef std::unique_ptr<BaseGameLogic> GameLogicPtr;

public:

    GameObject(const std::string& objectName, int entId);
    ~GameObject();

    void update();

    void addLogic(GameLogicPtr&& logic);
    template<typename T>
    T* getLogic() {
        for(auto& logic : logics) {
            if(logic->getTypeId() == GetTypeId<T>()) {
                return reinterpret_cast<T*>(logic.get());
            }
        }
        return nullptr;
    }

    const std::string& ET_getName() const override { return name; }
    EntityId getEntityId() const { return entityId; }

private:

    std::string name;
    std::vector<GameLogicPtr> logics;
    EntityId entityId;
};

#endif /* __GAME_OBJECT_HPP__ */