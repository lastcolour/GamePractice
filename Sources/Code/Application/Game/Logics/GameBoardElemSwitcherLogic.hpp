#ifndef __GAME_BOARD_SWITCH_LOGIC_HPP__
#define __GAME_BOARD_SWITCH_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Math/Transform.hpp"

class GameBoardElemSwitcherLogic : public EntityLogic,
    public ETNode<ETGameBoardElemSwitcher>,
    public ETNode<ETGameTimerEvents> {
public:

    GameBoardElemSwitcherLogic();
    virtual ~GameBoardElemSwitcherLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameBoardElemSwitcher
    void ET_switchBoardElems(EntityId firstId, EntityId secondId) override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

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