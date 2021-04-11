#ifndef __GAME_ELEM_ROCKET_LOGIC_HPP__
#define __GAME_ELEM_ROCKET_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"

class GameElemRocketLogic : public EntityLogic,
    public ETNode<ETGameBoardElemTriggerLogic> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameElemRocketLogic();
    virtual ~GameElemRocketLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemTriggerLogic
    void ET_start();
    bool ET_update(float dt) override;

private:

    Vec2i startPt;
    float speed;
    bool isHorizontal;
    float currTime;
};

#endif /* __GAME_ELEM_ROCKET_LOGIC_HPP__ */