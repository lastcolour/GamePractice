#ifndef __GAME_BOARD_ANIMATION_HPP__
#define __GAME_BOARD_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/ETGameElem.hpp"
#include "Math/Vector.hpp"

class ReflectContext;

class GameBoardAnimation : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardAnimation> {
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

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

private:

    Vec3 startScale;
    float scaleDuration;
    float minScale;
    float currDuration;
};

#endif /* __GAME_BOARD_ANIMATION_HPP__ */