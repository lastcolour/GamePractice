#ifndef __GAME_BOARD_ELEM_LOGIC_HPP__
#define __GAME_BOARD_ELEM_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class ReflectContext;

class GameBoardElemLogic : public EntityLogic {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardElemLogic();
    virtual ~GameBoardElemLogic();

    // EntityLogic
    bool init() override;
    void deinit() override;
};

#endif /* __GAME_BOARD_ELEMENT_LOGIC_HPP__ */