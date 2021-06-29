#ifndef __GAME_BOARD_UTILS_HPP__
#define __GAME_BOARD_UTILS_HPP__

#include "Game/ETGameElem.hpp"

namespace GameUtils {

const int BOARD_BACKGROUND_Z_OFFSET = 1;
const int ELEM_HIGHLIGHT_Z_OFFSET = 2;
const int BOARD_ELEM_Z_OFFSET = 3;

EBoardElemState GetElemState(EntityId elemId);

void SetElemState(EntityId elemId, EBoardElemState state);

EBoardElemType GetElemType(EntityId elemId);

bool HasTriggerLogic(EntityId elemId);

bool IsTriggerType(EBoardElemType elemType);

void TryTriggerElemDestroy(EntityId entId);

void TryTriggerElemDestroy(const Vec2i& boardPt);

void PlayElemDestroyEffect(EntityId elemId);

} // namespace GameUtils

#endif /* __GAME_BOARD_UTILS_HPP__ */