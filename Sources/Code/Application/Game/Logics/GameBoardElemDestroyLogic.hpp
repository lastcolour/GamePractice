#ifndef __GAME_BOARD_ELEM_DESTROY_LOGIC_HPP__
#define __GAME_BOARD_ELEM_DESTROY_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class ReflectContext;

class GameBoardElemDestroyLogic : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardElemDestroy> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardElemDestroyLogic();
    virtual ~GameBoardElemDestroyLogic();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameBoardElemDestroy
    void ET_destroyBoardElem(EntityId elemId) override;

private:

    struct DestroyTask {
        EntityId entId;
        float duration;
        Vec3 startScale;
    };

private:

    std::vector<DestroyTask> destroyTasks;
    float destroyDuration;
};

#endif /* __GAME_BOARD_ELEM_DESTROY_LOGIC_HPP__ */