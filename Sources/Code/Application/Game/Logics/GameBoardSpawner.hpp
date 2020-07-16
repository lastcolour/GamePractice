#ifndef __GAME_BOARD_SPAWNER_HPP__
#define __GAME_BOARD_SPAWNER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIBox.hpp"

class ReflectContext;

class GameBoardSpawner : public EntityLogic,
    public ETNode<ETUIBoxEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardSpawner();
    virtual ~GameBoardSpawner();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIBoxEvents
    void ET_onBoxResized(const AABB2Di& newAabb) override;
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onHidden() override;
    void ET_onShown() override;

private:

    void setGameBoard(const char* gameBoard);

private:

    std::string gameBoardName;
    EntityId gameBoardId;
};

#endif /* __GAME_BOARD_SPAWNER_HPP__ */