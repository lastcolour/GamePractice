#ifndef __GAME_BOARD_UTILS_HPP__
#define __GAME_BOARD_UTILS_HPP__

#include "Game/ETGameElem.hpp"

namespace GameUtils {

EBoardElemState GetElemState(EntityId elemId);

void SetElemState(EntityId elemId, EBoardElemState state);

EBoardElemType GetElemType(EntityId elemId); 

} // namespace GameUtils

#endif /* __GAME_BOARD_UTILS_HPP__ */