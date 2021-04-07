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
    void ET_createMergeTask(EntityId fromId, EntityId toId) override;
    bool ET_hasMergeTasks() const override;
    void ET_updateMergeTasks(float dt) override;

private:

    struct MergeTask {
        EntityId targetId;
        Vec2 dir;
        float vel;
    };

    struct MutateTask {
        EntityId targetId;
        float duration;
    };

private:

    void processMergeTasks(EntityId toId, std::vector<MergeTask>& tasks, float dt, float cellSize);
    void processMutateTasks(float dt);

private:

    std::unordered_map<EntityId, std::vector<MergeTask>> tasksMap;
    std::vector<MutateTask> mutateTasks;
    Math::RandomFloatGenerator floatGen;
    SoundEvent mergeSound;
    SoundEvent mutateSound;
    std::string vRocketEntity;
    std::string hRocketEntity;
    std::string bombEntity;
    std::string starEntity;
    EntityId mutateEffectId;
    float startVel;
    float acc;
    float mutateTaskDuration;
};

#endif /* __BOARD_ELEM_MERGE_MANAGER_HPP__ */