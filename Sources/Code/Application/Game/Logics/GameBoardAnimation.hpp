#ifndef __GAME_BOARD_ANIMATION_HPP__
#define __GAME_BOARD_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameBoardSpawner.hpp"

class GameBoardAnimation : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardAnimation>,
    public ETNode<ETGameBoardSpawnerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardAnimation();
    virtual ~GameBoardAnimation();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardAnimation
    void ET_zoomOut() override;

    // ETGameBoardSpawnerEvents
    void ET_onStartLoading() override;
    void ET_onStartDestroying() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

private:

    float scaleDuration;
    float minScale;
    float currDuration;
};

#endif /* __GAME_BOARD_ANIMATION_HPP__ */