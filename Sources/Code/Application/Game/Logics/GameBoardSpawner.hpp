#ifndef __GAME_BOARD_SPAWNER_HPP__
#define __GAME_BOARD_SPAWNER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "UI/ETUIBox.hpp"

class GameBoardSpawner : public EntityLogic,
    public ETNode<ETGameBoardSpawner>,
    public ETNode<ETUIElementEvents> {
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

    // ETUIElementEvents
    void ET_onBoxChanged(const AABB2D& newAabb) override;
    void ET_onZIndexChanged(int newZIndex) override {}
    void ET_onAlphaChanged(float newAlpha) override {}
    void ET_onHidden(bool flag) override {}
    void ET_onDisabled(bool flag) override {}
    void ET_onIngoreTransform(bool flag) override {}

private:

    std::string gameBoardName;
    EntityId gameBoardId;
};

#endif /* __GAME_BOARD_SPAWNER_HPP__ */