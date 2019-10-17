#include "Game/Logics/GameBoardInteractionLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

namespace {

const float MIN_MOVE_LEN_FOR_SWITCH = 0.6f;

} // namespace

GameBoardInteractionLogic::GameBoardInteractionLogic() :
    startPt(0),
    lastPt(0) {
}

GameBoardInteractionLogic::~GameBoardInteractionLogic() {
}

bool GameBoardInteractionLogic::serialize(const JSONNode& node) {
    (void)node;
    return true;
}

bool GameBoardInteractionLogic::init() {
    ETNode<ETGAmeBoardInteractionLogic>::connect(getEntityId());
    return true;
}

void GameBoardInteractionLogic::onEndElemMove(const Vec2i& endPt) {
    Transform tm;
    ET_SendEventReturn(tm, activeElemId, &ETEntity::ET_getTransform);
    Vec2 moveDir = Vec2(static_cast<float>(endPt.x), static_cast<float>(endPt.y)) - Vec2(tm.pt.x, tm.pt.y);

    Vec2i nextBoardPt = startPt;
    if(std::abs(moveDir.x) > std::abs(moveDir.y)) {
        if(moveDir.x > 0.f) {
            nextBoardPt.x = startPt.x + 1;
        } else {
            nextBoardPt.x = startPt.x - 1;
        }
    } else {
        if(moveDir.y > 0.f) {
            nextBoardPt.y = startPt.y + 1;
        } else {
            nextBoardPt.y = startPt.y - 1;
        }
    }

    int cellSize = 0;
    ET_SendEventReturn(cellSize, getEntityId(), &ETGameBoard::ET_getCellSize);
    if(moveDir.getLenght() < static_cast<float>(cellSize) * MIN_MOVE_LEN_FOR_SWITCH) {
        return;
    }
    EntityId nextElemId;
    ET_SendEventReturn(nextElemId, getEntityId(), &ETGameBoard::ET_getElemByBoardPos, nextBoardPt);
    if(!nextElemId.isValid()) {
        return;
    }
    EBoardElemState state = EBoardElemState::Void;
    ET_SendEventReturn(state, getEntityId(), &ETGameBoard::ET_getElemState, nextElemId);
    if(state != EBoardElemState::Static) {
        return;
    }
    ET_SendEvent(getEntityId(), &ETGameBoardElemSwitcher::ET_switchBoardElems, activeElemId, nextElemId);
}

void GameBoardInteractionLogic::ET_onTouch(EActionType actionType, const Vec2i& pt) {
    lastPt = pt;
    switch (actionType)
    {
    case EActionType::Press: {
        ET_SendEventReturn(activeElemId, getEntityId(), &ETGameBoard::ET_getElemByPos, pt);
        if(activeElemId.isValid()) {
            ET_SendEventReturn(startPt, getEntityId(), &ETGameBoard::ET_getElemBoardPos, activeElemId);
        }
        break;
    }
    case EActionType::Move: {
        break;
    }
    case EActionType::Release: {
        if(activeElemId.isValid()) {
            onEndElemMove(pt);
        }
        activeElemId = InvalidEntityId;
        break;
    }
    default:
        break;
    }
}

void GameBoardInteractionLogic::ET_allowInteraction(bool flag) {
    if(flag) {
        ETNode<ETInputEvents>::connect(getEntityId());
    } else {
        ETNode<ETInputEvents>::disconnect();
        activeElemId = InvalidEntityId;
        if(activeElemId.isValid()) {
            ET_onTouch(EActionType::Release, lastPt);
        }
    }
}

bool GameBoardInteractionLogic::ET_canInteract() const {
    return ET_IsExistNode<ETInputEvents>(getEntityId());
}