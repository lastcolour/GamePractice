#ifndef __GAME_END_TIMER_UPDATER_LOGIC_HPP__
#define __GAME_END_TIMER_UPDATER_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Core/ETPrimitives.hpp"

class ReflectContext;

class GameEndTimerUpdaterLogic : public EntityLogic,
    public ETNode<ETGameEndTimerUpdater> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameEndTimerUpdaterLogic();
    virtual ~GameEndTimerUpdaterLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameEndTimerUpdater
    virtual void ET_setEndTime(float endTime) override;
};

#endif /* __GAME_END_TIMER_UPDATER_LOGIC_HPP__ */