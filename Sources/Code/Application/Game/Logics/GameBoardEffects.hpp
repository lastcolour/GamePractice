#ifndef __GAME_BOARD_EFFECTS_HPP__
#define __GAME_BOARD_EFFECTS_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameBoard.hpp"

class GameBoardEffects : public EntityLogic,
    public ETNode<ETGameBoardEffects> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardEffects();
    virtual ~GameBoardEffects();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardEffects
    void ET_playDestroyEffect(EntityId elemId) override;
    void ET_playMergeEffect(EntityId elemId) override;
    void ET_playMutateEffect(EntityId elemId) override;
    EntityId ET_getRocketTrailEffect() const override;

public:

    EntityId redDestroyEffectId;
    EntityId blueDestroyEffectId;
    EntityId yellowDestroyEffectId;
    EntityId greenDestroyEffectId;
    EntityId purpleDestroyEffectId;
    EntityId bomdDestroyEffectId;
    EntityId rocketTrailEffectId;
    EntityId rocketDestroyEffectId;
    EntityId mutateEffectId;
    EntityId mergeEffectId;
    float destroyEffectScale;
};

#endif /* __GAME_BOARD_EFFECTS_HPP__ */