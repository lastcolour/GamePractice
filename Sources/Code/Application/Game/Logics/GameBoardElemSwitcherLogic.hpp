#ifndef __GAME_BOARD_SWITCH_LOGIC_HPP__
#define __GAME_BOARD_SWITCH_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Math/Transform.hpp"

class GameBoardElemSwitcherLogic : public EntityLogic,
    public ETNode<ETGameBoardElemSwitcher>,
    public ETNode<ETTimerEvents> {
public:

    GameBoardElemSwitcherLogic();
    virtual ~GameBoardElemSwitcherLogic();

    // EntityLogic
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