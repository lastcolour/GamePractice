#ifndef __GAME_BOARD_ELEM_LOGIC_HPP__
#define __GAME_BOARD_ELEM_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"
#include "Game/ETGameBoard.hpp"

class GameBoardElemLogic : public EntityLogic,
    public ETNode<ETGameBoardElem> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardElemLogic();
    virtual ~GameBoardElemLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElem
    void ET_setElemState(EBoardElemState newState) override;
    EBoardElemState ET_getState() const override;
    EBoardElemType ET_getType() const override;
    void ET_triggerDestroy() override;
    void ET_triggerLand() override;
    void ET_triggerMutate() override;
    void ET_setMutateAfterMerge(EPatternType pattern) override;
    void ET_triggerMergeTo(EntityId mergeTargetId) override;
    void ET_setSelected(bool flag) override;
    bool ET_canMatch() const override;
    bool ET_canSwitch() const override;
    void ET_onLandPlayed() override;
    void ET_onDestroyPlayed() override;
    void ET_onMergeDone(EntityId elemId) override;
    void ET_onTriggerDone() override;

private:

    EBoardElemState state;
    EBoardElemType type;
    EPatternType mutateTo;
};

#endif /* __GAME_BOARD_ELEMENT_LOGIC_HPP__ */