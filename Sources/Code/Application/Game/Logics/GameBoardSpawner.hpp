#ifndef __GAME_BOARD_SPAWNER_HPP__
#define __GAME_BOARD_SPAWNER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIBox.hpp"

class ReflectContext;

class GameBoardSpawner : public EntityLogic {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardSpawner();
    virtual ~GameBoardSpawner();

    // EntityLogic
    bool init() override;
    void deinit() override;

private:

    void setGameBoard(const char* gameBoard);

private:

    std::string gameBoardName;
    EntityId gameBoardId;
};

#endif /* __GAME_BOARD_SPAWNER_HPP__ */