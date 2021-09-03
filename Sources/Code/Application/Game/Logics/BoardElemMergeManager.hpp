#ifndef __BOARD_ELEM_MERGE_MANAGER_HPP__
#define __BOARD_ELEM_MERGE_MANAGER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Audio/SoundEvent.hpp"

class BoardElemMergeManager : public EntityLogic,
    public ETNode<ETGameBoardElemMergeManager>,
    public ETNode<ETGameBoardSpawnerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    BoardElemMergeManager();
    virtual ~BoardElemMergeManager();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemMergeManager
    void ET_createMergeTask(const ElemMergeTask& newTask) override;
    bool ET_hasMergeTasks() const override;
    void ET_updateMergeTasks(float dt) override;

    // ETGameBoardSpawnerEvents
    void ET_onStartLoading() override;
    void ET_onStartDestroying() override;

private:

    struct MergeTask {
        EntityId targetId;
        float vel;
    };

    struct MutateTask {
        EntityId toId;
        std::vector<MergeTask> merges;
        float duration;
        EBoardElemType elemType;
        float pulseScale;
    };

private:

    void processMerges(MutateTask& task, float dt, float cellSize);

private:

    std::vector<MutateTask> mutateTasks;
    float pulseScaleIncrese;
    float pulseScaleDecaySpeed;
    float shrinkSpeed;
    float startVel;
    float acc;
    float mutateTaskDuration;
};

#endif /* __BOARD_ELEM_MERGE_MANAGER_HPP__ */