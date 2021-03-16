#ifndef __GAME_BOARD_SPAWNER_HPP__
#define __GAME_BOARD_SPAWNER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "UI/ETUIBox.hpp"

class GameBoardSpawner : public EntityLogic,
    public ETNode<ETGameBoardSpawner> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardSpawner();
    virtual ~GameBoardSpawner();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardSpawner
    void ET_setPendingLevel(const std::string& levelName) override;
    void ET_loadPendingLevel() override;
    void ET_unloadLevel() override;
    const char* ET_getLevelName() const override;

private:

    std::string gameBoardName;
    EntityId gameBoardId;
};

#endif /* __GAME_BOARD_SPAWNER_HPP__ */