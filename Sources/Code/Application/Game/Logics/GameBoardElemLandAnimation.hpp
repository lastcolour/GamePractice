#ifndef __GAME_BOARD_ELEM_LAND_ANIMATION_HPP__
#define __GAME_BOARD_ELEM_LAND_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/ETGameElem.hpp"
#include "Audio/SoundEvent.hpp"
#include "Math/Vector.hpp"

class ReflectContext;

class GameBoardElemLandAnimation : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardElemLandAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardElemLandAnimation();
    virtual ~GameBoardElemLandAnimation();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemLandAnimation
    void ET_playLand() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

private:

    enum class State {
        Finished,
        ScaleDown,
        ScaleUp,
        ScaleBack
    };

private:

    void setLandSound(const char* soundEventName);

private:

    Vec2 elemLandPt;
    SoundEvent landSound;
    float totalDuration;
    float currDuration;
    State state;
};

#endif /* __GAME_BOARD_ELEM_LAND_ANIMATION_HPP__ */