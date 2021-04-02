#include "Game/Logics/GameBoardUtils.hpp"
#include "Game/ETGameElem.hpp"

namespace GameUtils {

EBoardElemState GetElemState(EntityId elemId) {
    EBoardElemState state = EBoardElemState::Destroyed;
    ET_SendEventReturn(state, elemId, &ETGameBoardElem::ET_getState);
    return state;
}

void SetElemState(EntityId elemId, EBoardElemState state) {
    ET_SendEvent(elemId, &ETGameBoardElem::ET_setElemState, state);
}

EBoardElemType GetElemType(EntityId elemId) {
    EBoardElemType type;
    ET_SendEventReturn(type, elemId, &ETGameBoardElem::ET_getType);
    return type;
}

} // namespace GameUtils