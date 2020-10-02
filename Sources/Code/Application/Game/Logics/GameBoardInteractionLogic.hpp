#ifndef __GAME_BOARD_INTERACTION_LOGIC_HPP__
#define __GAME_BOARD_INTERACTION_LOGIC_HPP__

#include "Core/ETPrimitives.hpp"
#include "Platform/ETSurface.hpp"
#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameTimer.hpp"
#include "Math/Transform.hpp"
#include "Audio/SoundEvent.hpp"

class ReflectContext;

class GameBoardInteractionLogic : public EntityLogic,
    public ETNode<ETInputEvents>,
    public ETNode<ETGameBoardInteractionLogic>,
    public ETNode<ETGameTimerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardInteractionLogic();
    virtual ~GameBoardInteractionLogic();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETInputEvents
    void ET_onTouch(EActionType actionType, const Vec2i& pt) override;
    void ET_onButton(EActionType actionType, EButtonId buttonId) override { (void)actionType; (void)buttonId; }

    // ETGameBoardInteractionLogic
    void ET_allowInteraction(bool flag) override;
    bool ET_canInteract() const override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

private:

    void tryFinishElemMove(const Vec2i& pt);
    void createSwitchElemsTask(EntityId firstId, EntityId secondId);
    void switchElements(EntityId firstId, EntityId secondId);
    void setSwitchSoundEvent(const char* soundName);
    void setActiveElem(EntityId elemId);

private:

    struct SwitchTask {
        EntityId firstId;
        Transform firstTm;
        EntityId secondId;
        Transform secondTm;
        float duration;
    };

private:

    Vec2i startPt;
    Vec2i lastPt;
    EntityId activeElemId;
    float switchDuration;
    std::vector<SwitchTask> switchTasks;
    SoundEvent switchSoundEvent;
};

#endif /* __GAME_BOARD_INTERACTION_LOGIC_HPP__ */