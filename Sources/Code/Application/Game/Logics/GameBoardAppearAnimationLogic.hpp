#ifndef __GAME_BOARD_APPEAR_ANIMATION_LOGIC_HPP__
#define __GAME_BOARD_APPEAR_ANIMATION_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameTimer.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"

class ReflectContext;

class GameBoardAppearAnimationLogic : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardAppearAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardAppearAnimationLogic();
    virtual ~GameBoardAppearAnimationLogic();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameBoardAppearAnimation
    void ET_startBoardAppearing() override;

private:

    void moveAllElemsOnTop();
    void moveCollumnElemDown(int collumnIdx, float duration);
    void alignSpeedWithScreenSize();

private:

    Vec2i boardSize;
    float currDuration;
    float fallSpeed;
    float shift;
    float collumnDelay;
    float duration;
};

#endif /* __GAME_BOARD_APPEAR_ANIMATION_LOGIC_HPP__ */