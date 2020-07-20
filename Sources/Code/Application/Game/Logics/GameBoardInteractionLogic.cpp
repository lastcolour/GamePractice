#include "Game/Logics/GameBoardInteractionLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Entity/ETEntity.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Audio/ETSound.hpp"

#include <algorithm>
#include <cassert>

namespace {

const float MIN_MOVE_LEN_FOR_SWITCH = 0.6f;

} // namespace

GameBoardInteractionLogic::GameBoardInteractionLogic() :
    startPt(0),
    lastPt(0),
    switchDuration(0.3f) {
}

GameBoardInteractionLogic::~GameBoardInteractionLogic() {
}

void GameBoardInteractionLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardInteractionLogic>("GameBoardInteraction")) {
        classInfo->addField("switchDuration", &GameBoardInteractionLogic::switchDuration);
        classInfo->addResourceField("switchSound", ResourceType::SoundEvent, &GameBoardInteractionLogic::setSwitchSoundEvent);
    }
}

bool GameBoardInteractionLogic::init() {
    ETNode<ETGAmeBoardInteractionLogic>::connect(getEntityId());
    ETNode<ETInputEvents>::connect(getEntityId());
    ETNode<ETGameTimerEvents>::connect(getEntityId());
    return true;
}

void GameBoardInteractionLogic::deinit() {
}

bool GameBoardInteractionLogic::tryFinishElemMove(const Vec2i& endPt) {
    EBoardElemState state = EBoardElemState::Void;
    ET_SendEventReturn(state, getEntityId(), &ETGameBoard::ET_getElemState, activeElemId);
    if(state != EBoardElemState::Static) {
        activeElemId = InvalidEntityId;
        return false;
    }

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
        return false;
    }
    EntityId nextElemId;
    ET_SendEventReturn(nextElemId, getEntityId(), &ETGameBoard::ET_getElemByBoardPos, nextBoardPt);
    if(!nextElemId.isValid()) {
        return false;
    }
    state = EBoardElemState::Void;
    ET_SendEventReturn(state, getEntityId(), &ETGameBoard::ET_getElemState, nextElemId);
    if(state != EBoardElemState::Static) {
        return false;
    }
    createSwitchElemsTask(activeElemId, nextElemId);
    return true;
}

void GameBoardInteractionLogic::ET_onTouch(EActionType actionType, const Vec2i& pt) {
    lastPt = pt;
    switch(actionType)
    {
    case EActionType::Press: {
        ET_SendEventReturn(activeElemId, getEntityId(), &ETGameBoard::ET_getElemByPos, pt);
        if(!activeElemId.isValid()) {
            break;
        }
        EBoardElemState state = EBoardElemState::Void;
        ET_SendEventReturn(state, getEntityId(), &ETGameBoard::ET_getElemState, activeElemId);
        if(state != EBoardElemState::Static) {
            activeElemId = InvalidEntityId;
            break;
        }
        ET_SendEventReturn(startPt, getEntityId(), &ETGameBoard::ET_getElemBoardPos, activeElemId);
        break;
    }
    case EActionType::Move: {
        if(activeElemId.isValid()) {
            if(tryFinishElemMove(pt)) {
                activeElemId = InvalidEntityId;
            }
        }
        break;
    }
    case EActionType::Release: {
        if(activeElemId.isValid()) {
            tryFinishElemMove(pt);
            activeElemId = InvalidEntityId;
        }
        break;
    }
    default:
        break;
    }
}

void GameBoardInteractionLogic::ET_onGameTick(float dt) {
    for(auto& task : switchTasks) {
        task.duration += dt;

        float prog = std::min(task.duration / switchDuration, 1.f);

        Transform newTm = task.firstTm;
        newTm.pt = Math::Lerp(task.firstTm.pt, task.secondTm.pt, prog);
        ET_SendEvent(task.firstId, &ETEntity::ET_setTransform, newTm);

        newTm = task.secondTm;
        newTm.pt = Math::Lerp(task.secondTm.pt, task.firstTm.pt, prog);
        ET_SendEvent(task.secondId, &ETEntity::ET_setTransform, newTm);
    }

    bool isNeedUpdateBoard = false;
    auto it = switchTasks.begin();
    while(it != switchTasks.end()) {
        auto& task = *it;
        if(task.duration > switchDuration) {
            ET_SendEvent(getEntityId(), &ETGameBoard::ET_switchElemsBoardPos, task.firstId, task.secondId);
            ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, task.firstId, EBoardElemState::Static);
            ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, task.secondId, EBoardElemState::Static);
            isNeedUpdateBoard = true;
            it = switchTasks.erase(it);
        } else {
            ++it;
        }
    }

    if(isNeedUpdateBoard) {
        ET_SendEvent(getEntityId(), &ETGameBoard::ET_updateBoard);
    }
}

void GameBoardInteractionLogic::createSwitchElemsTask(EntityId firstId, EntityId secondId) {
    assert(firstId != secondId && "Can't switch same element");

    if(switchSoundEvent) {
        switchSoundEvent->emit();
    }

    SwitchTask task;
    task.duration = 0.f;

    task.firstId = firstId;
    ET_SendEventReturn(task.firstTm, firstId, &ETEntity::ET_getTransform);
    ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, firstId, EBoardElemState::Switching);

    task.secondId = secondId;
    ET_SendEventReturn(task.secondTm, secondId, &ETEntity::ET_getTransform);
    ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, secondId, EBoardElemState::Switching);

    switchTasks.push_back(task);
}

void GameBoardInteractionLogic::ET_allowInteraction(bool flag) {
    if(flag) {
        ETNode<ETInputEvents>::connect(getEntityId());
    } else {
        ETNode<ETInputEvents>::disconnect();
        activeElemId = InvalidEntityId;
    }
}

bool GameBoardInteractionLogic::ET_canInteract() const {
    return ET_IsExistNode<ETInputEvents>(getEntityId());
}

void GameBoardInteractionLogic::setSwitchSoundEvent(const char* soundName) {
    ET_SendEventReturn(switchSoundEvent, &ETSoundManager::ET_createEvent, soundName);
}