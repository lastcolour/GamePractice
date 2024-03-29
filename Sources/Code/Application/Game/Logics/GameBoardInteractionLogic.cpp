#include "Game/Logics/GameBoardInteractionLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Render/ETRenderNode.hpp"
#include "Game/Logics/GameBoardUtils.hpp"
#include "Render/ETParticlesSystem.hpp"

#include <cassert>

namespace {

const float MIN_MOVE_LEN_FOR_SWITCH = 0.6f;

Vec3 getScaleDuringSwap(float prog, float maxScale) {
    float s = 1.f;
    if(prog < 0.5f) {
        s = Math::Lerp(1.f, maxScale, 2.f * prog);
    } else {
        s = Math::Lerp(maxScale, 1.f, 2.f * (prog - 0.5f));
    }
    return Vec3(s);
}

} // namespace

GameBoardInteractionLogic::GameBoardInteractionLogic() :
    startPt(0),
    maxScale(1.6f),
    switchDuration(0.3f) {
}

GameBoardInteractionLogic::~GameBoardInteractionLogic() {
}

void GameBoardInteractionLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardInteractionLogic>("GameBoardInteraction")) {
        classInfo->addField("switchDuration", &GameBoardInteractionLogic::switchDuration);
        classInfo->addField("maxScale", &GameBoardInteractionLogic::maxScale);
        classInfo->addField("switchSound", &GameBoardInteractionLogic::switchSoundEvent);
        classInfo->addField("swapEffectId", &GameBoardInteractionLogic::swapEffectId);
    }
}

void GameBoardInteractionLogic::init() {
    ETNode<ETGameBoardInteractionLogic>::connect(getEntityId());
    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ETNode<ETGameBoardSpawnerEvents>::connect(getEntityId());
}

void GameBoardInteractionLogic::deinit() {
}

bool GameBoardInteractionLogic::tryFinishElemMove(const Vec2i& endPt) {
    if(!activeElemId.isValid()) {
        return false;
    }
    bool canSwitch = false;
    ET_SendEventReturn(canSwitch, activeElemId, &ETGameBoardElem::ET_canSwitch);
    if(!canSwitch) {
        setActiveElem(InvalidEntityId);
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
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);
    if(moveDir.length() < static_cast<float>(cellSize) * MIN_MOVE_LEN_FOR_SWITCH) {
        return false;
    }
    EntityId nextElemId;
    ET_SendEventReturn(nextElemId, &ETGameBoard::ET_getElemByBoardPos, nextBoardPt);
    if(!nextElemId.isValid()) {
        return false;
    }
    canSwitch = false;
    ET_SendEventReturn(canSwitch, nextElemId, &ETGameBoardElem::ET_canSwitch);
    if(!canSwitch) {
        return false;
    }

    Vec2i swapDir = startPt - nextBoardPt;
    createSwitchElemsTask(activeElemId, nextElemId, swapDir);
    setActiveElem(InvalidEntityId);
    ET_SendEvent(&ETGameBoardInteractionEvents::ET_onElemMoved);
    return true;
}

void GameBoardInteractionLogic::ET_onTouch(const TouchEvent& event) {
    bool isBoardStatic = false;
    ET_SendEventReturn(isBoardStatic, &ETGameBoard::ET_isAllElemStatic);

    EntityId touchElemId;
    if(isBoardStatic && event.actionType != EActionType::ReleaseAndIgnore) {
        ET_SendEventReturn(touchElemId, &ETGameBoard::ET_getElemByPos, event.pt);
        doubleTapDetector.onTouchEvent(touchElemId, event);
    } else {
        setActiveElem(InvalidEntityId);
        doubleTapDetector.reset();
        return;
    }
    
    switch(event.actionType)
    {
    case EActionType::Press: {
        if(!touchElemId.isValid()) {
            break;
        }
        bool canSwitch = false;
        ET_SendEventReturn(canSwitch, touchElemId, &ETGameBoardElem::ET_canSwitch);
        if(!canSwitch) {
            break;
        }
        setActiveElem(touchElemId);
        break;
    }
    case EActionType::Move: {
        if(tryFinishElemMove(event.pt)) {
            doubleTapDetector.reset();
        }
        break;
    }
    case EActionType::Release: {
        if(tryFinishElemMove(event.pt)) {
            doubleTapDetector.reset();
            break;
        } else {
            setActiveElem(InvalidEntityId);
            auto tapInfo = doubleTapDetector.getTapInfo();
            if(tapInfo.tapCount == 2) {
                GameUtils::TryTriggerElemDestroy(InvalidEntityId, tapInfo.targetId);
                doubleTapDetector.reset();
            }
        }
        break;
    }
    case EActionType::ReleaseAndIgnore:
        [[fallthrough]];
    default:
        assert(false && "Invalid action type");
    }
}

void GameBoardInteractionLogic::ET_onGameTick(float dt) {
    for(auto& task : switchTasks) {
        task.duration += dt;

        float prog = std::min(task.duration / switchDuration, 1.f);

        Transform newTm = task.firstTm;
        newTm.pt = Math::Lerp(task.firstTm.pt, task.secondTm.pt, prog);
        if(task.dir.x > 0 || task.dir.y > 0) {
            newTm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), 2.f * prog * Math::PI);
        } else {
            newTm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), -2.f * prog * Math::PI);
        }
        newTm.scale = getScaleDuringSwap(prog, maxScale);
        ET_SendEvent(task.firstId, &ETEntity::ET_setLocalTransform, newTm);

        newTm = task.secondTm;
        newTm.quat.setIndentity();
        newTm.scale = Vec3(1.f);
        newTm.pt = Math::Lerp(task.secondTm.pt, task.firstTm.pt, prog);

        ET_SendEvent(task.secondId, &ETEntity::ET_setLocalTransform, newTm);
    }

    auto it = switchTasks.begin();
    while(it != switchTasks.end()) {
        auto& task = *it;
        if(task.duration > switchDuration) {

            GameUtils::SetElemState(task.firstId, EBoardElemState::Static);
            GameUtils::SetElemState(task.secondId, EBoardElemState::Static);

            int zIndex = 0;
            ET_SendEventReturn(zIndex, task.firstId, &ETRenderNode::ET_getZIndex);
            zIndex -= 1;
            ET_SendEvent(task.firstId, &ETRenderNode::ET_setZIndex, zIndex);

            ET_SendEvent(&ETGameBoard::ET_switchElemsBoardPos, task.firstId, task.secondId);
            if(GameUtils::HasTriggerLogic(task.firstId)) {
                ET_SendEvent(task.firstId, &ETGameBoardElemTriggerLogic::ET_setSwapedElem, task.secondId);
                GameUtils::TryTriggerElemDestroy(InvalidEntityId, task.firstId);
            } else if(GameUtils::HasTriggerLogic(task.secondId)) {
                ET_SendEvent(task.secondId, &ETGameBoardElemTriggerLogic::ET_setSwapedElem, task.firstId);
                GameUtils::TryTriggerElemDestroy(InvalidEntityId, task.secondId);
            }
            it = switchTasks.erase(it);
        } else {
            ++it;
        }
    }
}

void GameBoardInteractionLogic::createSwitchElemsTask(EntityId firstId, EntityId secondId, const Vec2i& swapDir) {
    assert(firstId != secondId && "Can't switch same element");

    switchSoundEvent.emit();
    ET_SendEvent(swapEffectId, &ETParticlesSystem::ET_emitTrackingEntity, firstId);

    SwitchTask task;
    task.duration = 0.f;

    int zIndex = 0;
    ET_SendEventReturn(zIndex, firstId, &ETRenderNode::ET_getZIndex);
    zIndex += 1;
    ET_SendEvent(firstId, &ETRenderNode::ET_setZIndex, zIndex);

    task.firstId = firstId;
    ET_SendEventReturn(task.firstTm, firstId, &ETEntity::ET_getLocalTransform);
    GameUtils::SetElemState(task.firstId, EBoardElemState::Switching);

    task.secondId = secondId;
    ET_SendEventReturn(task.secondTm, secondId, &ETEntity::ET_getLocalTransform);
    GameUtils::SetElemState(task.secondId, EBoardElemState::Switching);

    task.dir = swapDir;

    switchTasks.push_back(task);
}

void GameBoardInteractionLogic::ET_allowInteraction(bool flag) {
    if(flag) {
        ETNode<ETInputEvents>::connect(getEntityId());
    } else {
        ETNode<ETInputEvents>::disconnect();
        setActiveElem(InvalidEntityId);
        doubleTapDetector.reset();
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

bool GameBoardInteractionLogic::ET_hasActiveSwitching() const {
    return !switchTasks.empty();
}

void GameBoardInteractionLogic::ET_onStartLoading() {
    assert(!ET_hasActiveSwitching() && "Invalid switch state");
}

void GameBoardInteractionLogic::ET_onStartDestroying() {
    activeElemId = InvalidEntityId;
    switchTasks.clear();
    doubleTapDetector.reset();
}