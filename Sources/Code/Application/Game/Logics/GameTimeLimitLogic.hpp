#ifndef __GAME_TIME_LIMIT_LOGIC_HPP__
#define __GAME_TIME_LIMIT_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/ETGame.hpp"

class GameTimeLimitLogic : public EntityLogic,
    public ETNode<ETGameLimits>,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameStateEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameTimeLimitLogic();
    virtual ~GameTimeLimitLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameLimits
    void ET_setLabelId(EntityId newLabelId) override;
    EGameLimitType ET_getLimitType() const override;

    // ETGameStateEvents
    void ET_onGameEnterState(EGameState state) override;
    void ET_onGameLeaveState(EGameState state) override;

private:

    EntityId labelId;
    float gameDuration;
    float currTime;
};

#endif /* GameTimeLimitLogic */