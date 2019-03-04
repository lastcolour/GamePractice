#ifndef __GAME_LOGIC_HPP__
#define __GAME_LOGIC_HPP__

#include "Core/TypeId.hpp"
#include "Game/GameObject.hpp"

class JSONNode;

class BaseGameLogic {
public:

    BaseGameLogic() : gameObj(nullptr) {}
    virtual ~BaseGameLogic() = default;

    virtual bool init(const JSONNode& node) { return true; }
    virtual void update() {}
    virtual int getTypeId() const = 0;

    void setGameObject(const GameObject* obj) { gameObj = obj; }
    EntityId getEntityId() const {
        if(gameObj) {
            return gameObj->getEntityId();
        }
        return InvalidEntityId;
    }

private:

    const GameObject* gameObj;
};

template<typename T>
class GameLogic : public BaseGameLogic {
public:

    virtual ~GameLogic() = default;

public:

    int getTypeId() const override { return LOGIC_TYPE_ID; }

private:
    static int LOGIC_TYPE_ID;
};

template<typename T>
int GameLogic<T>::LOGIC_TYPE_ID = GetTypeId<T>();

#endif /* __GAME_LOGIC_HPP__ */