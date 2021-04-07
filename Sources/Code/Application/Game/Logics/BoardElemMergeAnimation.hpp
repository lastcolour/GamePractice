#ifndef __BOARD_ELEM_MERGE_ANIMATION_HPP__
#define __BOARD_ELEM_MERGE_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"
#include "Math/Random.hpp"
#include "Audio/SoundEvent.hpp"

class BoardElemMergeAnimation : public EntityLogic,
    public ETNode<ETGameBoardElemMergeAnimationManager> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    BoardElemMergeAnimation();
    virtual ~BoardElemMergeAnimation();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemMergeAnimationManager
    void ET_createMergeTask(EntityId fromId, EntityId toId) override;
    bool ET_hasMergeTasks() const override;
    void ET_updateMergeTasks(float dt) override;

private:

    struct MergeTask {
        EntityId targetId;
        Vec2 dir;
        float vel;
    };

private:

    void processMergeTasks(EntityId toId, std::vector<MergeTask>& tasks, float dt, float cellSize);

private:

    std::unordered_map<EntityId, std::vector<MergeTask>> tasksMap;
    Math::RandomFloatGenerator floatGen;
    SoundEvent mergeSound;
    float startVel;
    float acc;
};

#endif /* __BOARD_ELEM_MERGE_ANIMATION_HPP__ */