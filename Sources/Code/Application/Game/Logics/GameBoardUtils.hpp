#ifndef __GAME_BOARD_UTILS_HPP__
#define __GAME_BOARD_UTILS_HPP__

#include "Game/ETGameElem.hpp"

namespace GameUtils {

const int BACKGROUND_Z_INDEX = 0;
const int HIGHLIGHT_ELEM_Z_INDEX = 1;
const int ELEMENT_Z_INDEX = 2;
const int DESTROY_EFFECT_Z_INDEX = 3;

EBoardElemState GetElemState(EntityId elemId);

void SetElemState(EntityId elemId, EBoardElemState state);

EBoardElemType GetElemType(EntityId elemId); 

} // namespace GameUtils

#endif /* __GAME_BOARD_UTILS_HPP__ */