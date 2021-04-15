#ifndef __GAME_ELEM_STAR_LOGIC_HPP__
#define __GAME_ELEM_STAR_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"

class GameElemStarLogic : public EntityLogic,
    public ETNode<ETGameBoardElemTriggerLogic> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameElemStarLogic();
    virtual ~GameElemStarLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemTriggerLogic
    void ET_start();
    bool ET_update(float dt) override;
    void ET_setSwapedElem(EntityId elemId) override;

private:

    struct DestroyTask {
        EntityId entId;
        float delay;
    };

private:

    std::vector<DestroyTask> destroyTasks;
    Vec2i startPt;
    float destroyDelay;
    EBoardElemType elemType;
};

#endif /* __GAME_ELEM_STAR_LOGIC_HPP__ */