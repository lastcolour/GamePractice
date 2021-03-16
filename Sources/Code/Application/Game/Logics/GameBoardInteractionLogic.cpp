#include "Game/Logics/GameBoardInteractionLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Audio/ETSound.hpp"
#include "Game/ETGameElem.hpp"
#include "Render/ETRenderNode.hpp"

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

void GameBoardInteractionLogic::init() {
    ETNode<ETGameBoardInteractionLogic>::connect(getEntityId());
    ETNode<ETGameTimerEvents>::connect(getEntityId());
}

void GameBoardInteractionLogic::deinit() {
}

void GameBoardInteractionLogic::tryFinishElemMove(const Vec2i& endPt) {
    if(!activeElemId.isValid()) {
        return;
    }
    bool canSwitch = false;
    ET_SendEventReturn(canSwitch, activeElemId, &ETGameBoardElem::ET_canSwitch);
    if(!canSwitch) {
        setActiveElem(InvalidEntityId);
        return;
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
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);
    if(moveDir.getLenght() < static_cast<float>(cellSize) * MIN_MOVE_LEN_FOR_SWITCH) {
        return;
    }
    EntityId nextElemId;
    ET_SendEventReturn(nextElemId, &ETGameBoard::ET_getElemByBoardPos, nextBoardPt);
    if(!nextElemId.isValid()) {
        return;
    }
    canSwitch = false;
    ET_SendEventReturn(canSwitch, nextElemId, &ETGameBoardElem::ET_canSwitch);
    if(!canSwitch) {
        return;
    }
    createSwitchElemsTask(activeElemId, nextElemId);
    setActiveElem(InvalidEntityId);
    return;
}

void GameBoardInteractionLogic::ET_onTouch(EActionType actionType, const Vec2i& pt) {
    lastPt = pt;
    switch(actionType)
    {
    case EActionType::Press: {
        EntityId elemId;
        ET_SendEventReturn(elemId, &ETGameBoard::ET_getElemByPos, pt);
        if(!elemId.isValid()) {
            break;
        }
        bool canSwitch = false;
        ET_SendEventReturn(canSwitch, elemId, &ETGameBoardElem::ET_canSwitch);
        if(!canSwitch) {
            break;
        }
        setActiveElem(elemId);
        break;
    }
    case EActionType::Move: {
        tryFinishElemMove(pt);
        break;
    }
    case EActionType::Release: {
        tryFinishElemMove(pt);
        setActiveElem(InvalidEntityId);
        break;
    }
    case EActionType::ReleaseAndIgnore: {
        setActiveElem(InvalidEntityId);
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
        ET_SendEvent(task.firstId, &ETEntity::ET_setLocalTransform, newTm);

        newTm = task.secondTm;
        newTm.pt = Math::Lerp(task.secondTm.pt, task.firstTm.pt, prog);
        ET_SendEvent(task.secondId, &ETEntity::ET_setLocalTransform, newTm);
    }

    bool isNeedUpdateBoard = false;
    auto it = switchTasks.begin();
    while(it != switchTasks.end()) {
        auto& task = *it;
        if(task.duration > switchDuration) {
            ET_SendEvent(&ETGameBoard::ET_switchElemsBoardPos, task.firstId, task.secondId);

            int drawPriority = 0;
            ET_SendEventReturn(drawPriority, task.firstId, &ETRenderNode::ET_getDrawPriority);
            drawPriority -= 1;
            ET_SendEvent(task.firstId, &ETRenderNode::ET_setDrawPriority, drawPriority);

            ET_SendEvent(task.firstId, &ETGameBoardElem::ET_setElemState, EBoardElemState::Static);
            ET_SendEvent(task.secondId, &ETGameBoardElem::ET_setElemState, EBoardElemState::Static);
            isNeedUpdateBoard = true;
            it = switchTasks.erase(it);
        } else {
            ++it;
        }
    }

    if(isNeedUpdateBoard) {
        ET_SendEvent(&ETGameBoard::ET_matchElements);
    }
}

void GameBoardInteractionLogic::createSwitchElemsTask(EntityId firstId, EntityId secondId) {
    assert(firstId != secondId && "Can't switch same element");

    switchSoundEvent.emit();

    SwitchTask task;
    task.duration = 0.f;

    int drawPriority = 0;
    ET_SendEventReturn(drawPriority, firstId, &ETRenderNode::ET_getDrawPriority);
    drawPriority += 1;
    ET_SendEvent(firstId, &ETRenderNode::ET_setDrawPriority, drawPriority);

    task.firstId = firstId;
    ET_SendEventReturn(task.firstTm, firstId, &ETEntity::ET_getLocalTransform);
    ET_SendEvent(task.firstId, &ETGameBoardElem::ET_setElemState, EBoardElemState::Switching);

    task.secondId = secondId;
    ET_SendEventReturn(task.secondTm, secondId, &ETEntity::ET_getLocalTransform);
    ET_SendEvent(task.secondId, &ETGameBoardElem::ET_setElemState, EBoardElemState::Switching);

    switchTasks.push_back(task);
}

void GameBoardInteractionLogic::ET_allowInteraction(bool flag) {
    if(flag) {
        ETNode<ETInputEvents>::connect(getEntityId());
    } else {
        ETNode<ETInputEvents>::disconnect();
        setActiveElem(InvalidEntityId);
    }
}

void GameBoardInteractionLogic::setActiveElem(EntityId elemId) {
    if(activeElemId == elemId) {
        return;
    }
    if(elemId.isValid()) {
        ET_SendEventReturn(startPt, &ETGameBoard::ET_getElemBoardPos, elemId);
        ET_SendEvent(elemId, &ETGameBoardElem::ET_setSelected, true);
    } else {
        startPt = Vec2i(-1);
    }
    if(activeElemId.isValid()) {
        ET_SendEvent(activeElemId, &ETGameBoardElem::ET_setSelected, false);
    }
    activeElemId = elemId;
}

bool GameBoardInteractionLogic::ET_canInteract() const {
    return ET_IsExistNode<ETInputEvents>(getEntityId());
}

void GameBoardInteractionLogic::setSwitchSoundEvent(const char* soundName) {
    ET_SendEventReturn(switchSoundEvent, &ETSoundManager::ET_createEvent, soundName);
}