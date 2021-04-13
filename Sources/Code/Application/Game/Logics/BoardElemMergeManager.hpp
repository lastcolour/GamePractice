#ifndef __BOARD_ELEM_MERGE_MANAGER_HPP__
#define __BOARD_ELEM_MERGE_MANAGER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"
#include "Math/Random.hpp"
#include "Audio/SoundEvent.hpp"

class BoardElemMergeManager : public EntityLogic,
    public ETNode<ETGameBoardElemMergeManager> {
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

private:

    struct MergeTask {
        Vec2 dir;
        EntityId targetId;
        float vel;
    };

    struct MutateTask {
        EntityId toId;
        std::vector<MergeTask> merges;
        float duration;
        EBoardElemType elemType;
    };

private:

    void processMerges(MutateTask& task, float dt, float cellSize);
    void processMutate(MutateTask& task, float dt);

private:

    std::vector<MutateTask> mutateTasks;
    Math::RandomFloatGenerator floatGen;
    SoundEvent mergeSound;
    SoundEvent mutateSound;

    EntityId mutateEffectId;
    float startVel;
    float acc;
    float mutateTaskDuration;
};

#endif /* __BOARD_ELEM_MERGE_MANAGER_HPP__ */