#ifndef __GAME_END_TIMER_UPDATER_LOGIC_HPP__
#define __GAME_END_TIMER_UPDATER_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameEndTimerUpdaterLogic : public GameLogic,
    public ETNode<ETGameEndTimerUpdater> {
public:
    GameEndTimerUpdaterLogic();
    virtual ~GameEndTimerUpdaterLogic();

    // GameLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameEndTimerUpdater
    virtual void ET_setEndTime(float endTime) override;
};

#endif /* __GAME_END_TIMER_UPDATER_LOGIC_HPP__ */