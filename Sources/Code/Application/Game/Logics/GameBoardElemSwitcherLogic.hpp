#ifndef __GAME_BOARD_SWITCH_LOGIC_HPP__
#define __GAME_BOARD_SWITCH_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

class GameBoardElemSwitcherLogic : public GameLogic,
    public ETNode<ETGameBoardElemSwitcher>,
    public ETNode<ETTimerEvents> {
public:

    GameBoardElemSwitcherLogic();
    virtual ~GameBoardElemSwitcherLogic();

    // GameLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameBoardElemSwitcher
    void ET_switchBoardElems(EntityId firstId, EntityId secondId) override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

private:

    void switchElements(EntityId firstId, EntityId secondId);

private:

    struct SwitchTask {
        EntityId firstId;
        Transform firstTm;
        EntityId secondId;
        Transform secondTm;
        float duration;
    };

private:

    float switchDuration;
    std::vector<SwitchTask> switchTasks;
};

#endif /* __GAME_BOARD_SWITCH_LOGIC_HPP__ */