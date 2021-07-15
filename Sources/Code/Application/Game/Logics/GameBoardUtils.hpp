#ifndef __GAME_BOARD_UTILS_HPP__
#define __GAME_BOARD_UTILS_HPP__

#include "Game/ETGameElem.hpp"

namespace GameUtils {

const int BOARD_BACKGROUND_Z_OFFSET = 1;
const int ELEM_HIGHLIGHT_Z_OFFSET = 2;
const int BOARD_ELEM_Z_OFFSET = 3;
const int ROCKET_PART_Z_OFFSET = 4;

EBoardElemState GetElemState(EntityId elemId);

void SetElemState(EntityId elemId, EBoardElemState state);

EBoardElemType GetElemType(EntityId elemId);

bool HasTriggerLogic(EntityId elemId);

bool IsTriggerType(EBoardElemType elemType);

void TryTriggerElemDestroy(EntityId sourceId, EntityId entId);

void TryTriggerElemDestroy(EntityId sourceId, const Vec2i& boardPt);

void PlayElemDestroyEffect(EntityId elemId);

bool ShouldApplyTriggerDelay(EntityId targetId, EntityId sourceId);

} // namespace GameUtils

#endif /* __GAME_BOARD_UTILS_HPP__ */