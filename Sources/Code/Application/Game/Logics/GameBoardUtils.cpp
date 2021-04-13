#include "Game/Logics/GameBoardUtils.hpp"
#include "Game/ETGameBoard.hpp"

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

bool HasTriggerLogic(EntityId elemId) {
    auto elemType = GetElemType(elemId);
    return IsTriggerType(elemType);
}

bool IsTriggerType(EBoardElemType elemType) {
    bool res = false;
    switch(elemType) {
        case EBoardElemType::HRocket: {
            res = true;
            break;
        }
        case EBoardElemType::VRocket: {
            res = true;
            break;
        }
        case EBoardElemType::Bomb: {
            res = true;
            break;
        }
        case EBoardElemType::Star: {
            res = true;
            break;
        }
        default: {
            res = false;
        }
    }
    return res;
}

void TryTriggerElemDestroy(const Vec2i& boardPt) {
    EntityId elemId;
    ET_SendEventReturn(elemId, &ETGameBoard::ET_getElemByBoardPos, boardPt);
    if(!elemId.isValid()) {
        return;
    }
    auto elemState = GameUtils::GetElemState(elemId);
    if(elemState != EBoardElemState::Static) {
        return;
    }
    ET_SendEvent(elemId, &ETGameBoardElem::ET_triggerDestroy);
}

} // namespace GameUtils