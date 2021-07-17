#ifndef __GAME_BOARD_INTERACTION_LOGIC_HPP__
#define __GAME_BOARD_INTERACTION_LOGIC_HPP__

#include "Platform/ETSurface.hpp"
#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Audio/SoundEvent.hpp"

class GameBoardInteractionLogic : public EntityLogic,
    public ETNode<ETInputEvents>,
    public ETNode<ETGameBoardInteractionLogic>,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardSpawnerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardInteractionLogic();
    virtual ~GameBoardInteractionLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETInputEvents
    void ET_onTouch(EActionType actionType, const Vec2i& pt) override;
    void ET_onButton(EActionType actionType, EButtonId buttonId) override { (void)actionType; (void)buttonId; }

    // ETGameBoardInteractionLogic
    void ET_allowInteraction(bool flag) override;
    bool ET_canInteract() const override;
    bool ET_hasActiveSwitching() const override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameBoardSpawnerEvents
    void ET_onStartLoading() override;
    void ET_onStartDestroying() override;

private:

    void tryFinishElemMove(const Vec2i& pt);
    void createSwitchElemsTask(EntityId firstId, EntityId secondId, const Vec2i& swapDir);
    void switchElements(EntityId firstId, EntityId secondId);
    void setActiveElem(EntityId elemId);

private:

    struct SwitchTask {
        EntityId firstId;
        Transform firstTm;
        EntityId secondId;
        Transform secondTm;
        float duration;
        Vec2i dir;
    };

private:

    Vec2i startPt;
    EntityId activeElemId;
    float maxScale;
    float switchDuration;
    std::vector<SwitchTask> switchTasks;
    SoundEvent switchSoundEvent;
    EntityId swapEffectId;
};

#endif /* __GAME_BOARD_INTERACTION_LOGIC_HPP__ */