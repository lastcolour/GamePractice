#ifndef __GAME_BOARD_ELEM_DESTROY_LOGIC_HPP__
#define __GAME_BOARD_ELEM_DESTROY_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

class GameBoardElemDestroyLogic : public EntityLogic,
    public ETNode<ETTimerEvents>,
    public ETNode<ETGameBoardElemDestroy> {
public:

    GameBoardElemDestroyLogic();
    virtual ~GameBoardElemDestroyLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

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