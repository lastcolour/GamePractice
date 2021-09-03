#ifndef __BOARD_ELEM_DESTROY_ANIMATION_HPP__
#define __BOARD_ELEM_DESTROY_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameTimer.hpp"
#include "Audio/SoundEvent.hpp"

class BoardElemDestroyAnimation : public EntityLogic,
    public ETNode<ETBoardElemDestroyAnimation>,
    public ETNode<ETGameTimerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    BoardElemDestroyAnimation();
    virtual ~BoardElemDestroyAnimation();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETBoardElemDestroyAnimation
    void ET_playDestroy() override;

private:

    enum class State {
        Starting,
        Animating,
        Finishing,
        Ended
    };

private:

    float startDelay;
    float duration;
    float endDelay;
    float currDuration;
    State currState;
};

#endif /* __BOARD_ELEM_DESTROY_ANIMATION_HPP__ */