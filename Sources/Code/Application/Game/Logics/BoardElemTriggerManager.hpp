#ifndef __BOARD_ELEM_TRIGGER_MANAGER_HPP__
#define __BOARD_ELEM_TRIGGER_MANAGER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"
#include "Game/ETGameBoardSpawner.hpp"

class BoardElemTriggerManager : public EntityLogic,
    public ETNode<ETGameBoardElemTriggerManager>,
    public ETNode<ETGameBoardSpawnerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    BoardElemTriggerManager();
    virtual ~BoardElemTriggerManager();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemTriggerManager
    void ET_createTriggerTask(EntityId elemId, bool applyDelay) override;
    void ET_updateTriggerTasks(float dt) override;
    bool ET_hasTriggerTasks() const override;

    // ETGameBoardSpawnerEvents
    void ET_onStartLoading() override;
    void ET_onStartDestroying() override;

private:

    struct TriggerTask {
        EntityId entId;
        float delay;
        bool destroyPlayed;
    };

private:

    std::vector<TriggerTask> newTriggerTasks;
    std::vector<TriggerTask> triggerTasks;
    float triggerDelay;
};

#endif /* __BOARD_ELEM_TRIGGER_MANAGER_HPP__ */