#ifndef __GAME_ELEM_BOMB_LOGIC_HPP__
#define __GAME_ELEM_BOMB_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElem.hpp"

class GameElemBombLogic : public EntityLogic,
    public ETNode<ETGameBoardElemTriggerLogic> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameElemBombLogic();
    virtual ~GameElemBombLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemTriggerLogic
    void ET_start();
    bool ET_update(float dt) override;

private:

    Vec2i startPt;
    float speed;
    float currTime;
    int radius;
};

#endif /* __GAME_ELEM_BOMB_LOGIC_HPP__ */