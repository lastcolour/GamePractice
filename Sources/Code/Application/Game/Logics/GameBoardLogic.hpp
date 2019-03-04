#ifndef __GAME_BOARD_LOGIC_HPP__
#define __GAME_BOARD_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Math/AABB.hpp"

#include <vector>
#include <memory>

class GameObject;

class GameBoardLogic : public GameLogic<GameBoardLogic> {

    typedef std::unique_ptr<GameObject> GameObjectPtr;

public:

    GameBoardLogic();
    virtual ~GameBoardLogic();

    bool init(const JSONNode& node) override;
    void update() override;

private:

    bool serialize(const JSONNode& node);

private:

    int width;
    int height;
    float space;
    float cellScale;
    std::string cellObject;
    std::vector<GameObjectPtr> boardObjects;
};

#endif /* __GAME_BOARD_LOGIC_HPP__ */