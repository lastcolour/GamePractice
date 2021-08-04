#ifndef __GAME_BOARD_SHAKE_LOGIC_HPP__
#define __GAME_BOARD_SHAKE_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/ETGameBoard.hpp"

class GameBoardShakeLogic : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardShake> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardShakeLogic();
    virtual ~GameBoardShakeLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameBoardShake
    void ET_addShake() override;

private:

    std::vector<Vec2> shakes;
    float totalDuration;
    float currentDuration;
    float currentScale;
};

#endif /* __GAME_BOARD_SHAKE_LOGIC_HPP__ */