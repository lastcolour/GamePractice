#ifndef __GAME_BOARD_UTILS_HPP__
#define __GAME_BOARD_UTILS_HPP__

#include "Game/ETGameElem.hpp"

namespace GameUtils {

EBoardElemState GetElemState(EntityId elemId);

void SetElemState(EntityId elemId, EBoardElemState state);

EBoardElemType GetElemType(EntityId elemId);

bool HasTriggerLogic(EntityId elemId);

bool IsTriggerType(EBoardElemType elemType);

void TryTriggerElemDestroy(EntityId entId);

void TryTriggerElemDestroy(const Vec2i& boardPt);

} // namespace GameUtils

#endif /* __GAME_BOARD_UTILS_HPP__ */