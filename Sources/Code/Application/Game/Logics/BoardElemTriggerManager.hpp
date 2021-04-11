#ifndef __BOARD_ELEM_TRIGGER_MANAGER_HPP__
#define __BOARD_ELEM_TRIGGER_MANAGER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"

class BoardElemTriggerManager : public EntityLogic,
    public ETNode<ETGameBoardElemTriggerManager> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    BoardElemTriggerManager();
    virtual ~BoardElemTriggerManager();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemTriggerManager
    void ET_createTriggerTask(EntityId elemId) override;
    void ET_updateTriggerTasks(float dt) override;
    bool ET_hasTriggerTasks() const override;

private:

    std::vector<EntityId> triggerTasks;
};

#endif /* __BOARD_ELEM_TRIGGER_MANAGER_HPP__ */