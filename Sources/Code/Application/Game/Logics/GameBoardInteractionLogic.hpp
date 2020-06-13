#ifndef __GAME_BOARD_INTERACTION_LOGIC_HPP__
#define __GAME_BOARD_INTERACTION_LOGIC_HPP__

#include "Core/ETPrimitives.hpp"
#include "Core/ETSurface.hpp"
#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameBoardInteractionLogic : public EntityLogic,
    public ETNode<ETInputEvents>,
    public ETNode<ETGAmeBoardInteractionLogic> {
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

    // ETGAmeBoardInteractionLogic
    void ET_allowInteraction(bool flag) override;
    bool ET_canInteract() const override;

private:

    void onStartElemMove();
    void onElemMove();
    void onEndElemMove(const Vec2i& pt);

private:

    Vec2i startPt;
    Vec2i lastPt;
    EntityId activeElemId;
};

#endif /* __GAME_BOARD_INTERACTION_LOGIC_HPP__ */