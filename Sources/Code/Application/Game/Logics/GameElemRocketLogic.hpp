#ifndef __GAME_ELEM_ROCKET_LOGIC_HPP__
#define __GAME_ELEM_ROCKET_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"

class GameElemRocketLogic : public EntityLogic,
    public ETNode<ETGameBoardElemTriggerLogic>,
    public ETNode<ETGameBoardRenderElem> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameElemRocketLogic();
    virtual ~GameElemRocketLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemTriggerLogic
    void ET_start() override;
    bool ET_update(float dt) override;
    void ET_setSwapedElem(EntityId elemId) override;

    // ETGameBoardRenderElem
    void ET_initRender(EntityId rootRenderEntId, const Vec2& elemSize) override;
    void ET_deinitRender(EntityId rootRenderEntId) override;

private:

    void onStart();
    bool onUpdate(float dt);

private:

    Vec2i startPt;
    EntityId firstRocket;
    EntityId secondRocket;
    float speed;
    float currTime;
    float acc;
    float currSpeed;
    int prevOffset;
    bool isHorizontal;
    bool isStarted;
};

#endif /* __GAME_ELEM_ROCKET_LOGIC_HPP__ */