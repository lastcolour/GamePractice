#ifndef __GAME_BOARD_ELEM_LOGIC_HPP__
#define __GAME_BOARD_ELEM_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"
#include "Game/ETGameBoard.hpp"

class GameBoardElemLogic : public EntityLogic,
    public ETNode<ETGameBoardElem>,
    public ETNode<ETGameBoardRenderElem> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardElemLogic();
    virtual ~GameBoardElemLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElem
    void ET_setState(EBoardElemState newState) override;
    EBoardElemState ET_getState() const override;
    void ET_setType(EBoardElemType newType) override;
    EBoardElemType ET_getType() const override;
    void ET_triggerDestroy(EntityId sourceId) override;
    void ET_triggerLand() override;
    void ET_setSelected(bool flag) override;
    bool ET_canMatch() const override;
    bool ET_canSwitch() const override;
    void ET_onLandPlayed() override;
    void ET_onDestroyDone() override;
    void ET_onTriggerDone() override;
    void ET_onMergeDone() override;

    // ETGameBoardRenderElem
    void ET_initRender(const Vec2& elemSize) override;
    void ET_deinitRender() override;

private:

    EBoardElemState state;
    EBoardElemType type;
};

#endif /* __GAME_BOARD_ELEMENT_LOGIC_HPP__ */