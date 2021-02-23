#ifndef __BOARD_ELEM_DESTROY_ANIMATION_HPP__
#define __BOARD_ELEM_DESTROY_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Math/Transform.hpp"
#include "Game/ETGameTimer.hpp"
#include "Audio/SoundEvent.hpp"

class ReflectContext;

class BoardElemDestroyAnimation : public EntityLogic,
    public ETNode<ETBoardElemDetroyAnimation>,
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

    // ETBoardElemDetroyAnimation
    void ET_playDestroy() override;

private:

    void setDestroySoundEvent(const char* eventName);

private:

    SoundEvent destroySound;
    float startDelay;
    float duration;
    float currDuration;
};

#endif /* __BOARD_ELEM_DESTROY_ANIMATION_HPP__ */