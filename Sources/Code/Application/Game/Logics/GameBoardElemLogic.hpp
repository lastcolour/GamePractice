#ifndef __GAME_BOARD_ELEM_LOGIC_HPP__
#define __GAME_BOARD_ELEM_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameBoardElemLogic : public EntityLogic {
public:

    GameBoardElemLogic();
    virtual ~GameBoardElemLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;
};

#endif /* __GAME_BOARD_ELEMENT_LOGIC_HPP__ */