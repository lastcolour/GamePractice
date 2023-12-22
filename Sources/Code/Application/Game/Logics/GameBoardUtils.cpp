#include "Game/Logics/GameBoardUtils.hpp"
#include "Game/ETGameBoard.hpp"
#include "Render/ETRenderNode.hpp"

namespace GameUtils {

EBoardElemState GetElemState(EntityId elemId) {
    EBoardElemState state = EBoardElemState::Destroyed;
    ET_SendEventReturn(state, elemId, &ETGameBoardElem::ET_getState);
    return state;
}

void SetElemState(EntityId elemId, EBoardElemState state) {
    ET_SendEvent(elemId, &ETGameBoardElem::ET_setState, state);
}

EBoardElemType GetElemType(EntityId elemId) {
    EBoardElemType type = EBoardElemType::None;
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

void TryTriggerElemDestroy(EntityId sourceId, EntityId entId) {
    if(!entId.isValid()) {
        return;
    }
    auto elemState = GameUtils::GetElemState(entId);
    if(elemState != EBoardElemState::Static) {
        return;
    }
    ET_SendEvent(entId, &ETGameBoardElem::ET_triggerDestroy, sourceId);
}

void TryTriggerElemDestroy(EntityId sourceId, const Vec2i& boardPt) {
    Vec2i boardSize(0);
    ET_SendEventReturn(boardSize, &ETGameBoard::ET_getBoardSize);
    if(boardPt.x < 0 || boardPt.x >= boardSize.x
        || boardPt.y < 0 || boardPt.y >= boardSize.y) {
        return;
    }

    EntityId elemId;
    ET_SendEventReturn(elemId, &ETGameBoard::ET_getElemByBoardPos, boardPt);
    if(elemId.isValid()) {
        TryTriggerElemDestroy(sourceId, elemId);
    } else {
        ET_SendEvent(&ETGameBoardElemHighlighter::ET_highlightCell, boardPt);
    }
}

void PlayElemDestroyEffect(EntityId elemId) {
    if(!elemId.isValid()) {
        return;
    }
    ET_SendEvent(&ETGameBoardEffects::ET_playDestroyEffect, elemId);

    Vec2i elemCellPt(-1);
    ET_SendEventReturn(elemCellPt, &ETGameBoard::ET_getElemBoardPos, elemId);

    if(elemCellPt != Vec2i(-1)) {
        ET_SendEvent(&ETGameBoardElemHighlighter::ET_highlightCell, elemCellPt);
    }
}

bool ShouldApplyTriggerDelay(EntityId targetId, EntityId sourceId) {
    EBoardElemType sType = EBoardElemType::None;
    ET_SendEventReturn(sType, sourceId, &ETGameBoardElem::ET_getType);

    EBoardElemType tType = EBoardElemType::None;
    ET_SendEventReturn(tType, targetId, &ETGameBoardElem::ET_getType);

    if(sType == EBoardElemType::Bomb && tType == sType) {
        return true;
    }

    return false;
}

} // namespace GameUtils