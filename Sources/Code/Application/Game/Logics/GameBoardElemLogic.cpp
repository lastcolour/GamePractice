#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderNode.hpp"
#include "Core/ETLogger.hpp"
#include "Game/ETGameInterfaces.hpp"

namespace {

ColorB GetElemColorForType(EBoardElemType elemType) {
    ColorB retCol(255, 255, 255);
    switch(elemType) {
        case EBoardElemType::Red:
            retCol = ColorB(255, 0, 0);
            break;
        case EBoardElemType::Blue:
            retCol = ColorB(0, 0, 255);
            break;
        case EBoardElemType::Green:
            retCol = ColorB(0, 255, 0);
            break;
        case EBoardElemType::Purple:
            retCol = ColorB(255, 0, 255);
            break;
        case EBoardElemType::Yellow:
            retCol = ColorB(255, 255, 0);
            break;
        default:
            break;
    }
    return retCol;
}

} // namespace

GameBoardElemLogic::GameBoardElemLogic() :
    lifeState(EBoardElemLifeState::Void),
    moveState(EBoardElemMoveState::Static),
    type(EBoardElemType::Yellow) {
}

GameBoardElemLogic::~GameBoardElemLogic() {
}

void GameBoardElemLogic::Reflect(ReflectContext& ctx) {
    ctx.classInfo<GameBoardElemLogic>("GameBoardElem");
}

bool GameBoardElemLogic::init() {
    ETNode<ETGameBoardElem>::connect(getEntityId());
    ETNode<ETBoardElemDetroyAnimationEvents>::connect(getEntityId());
    ET_setType(type);
    return true;
}

void GameBoardElemLogic::deinit() {
}

void GameBoardElemLogic::ET_setMoveState(EBoardElemMoveState newState) {
    moveState = newState;
}

EBoardElemMoveState GameBoardElemLogic::ET_getMoveState() const {
    return moveState;
}

void GameBoardElemLogic::ET_setLifeState(EBoardElemLifeState newState) {
    lifeState = newState;
    if(lifeState == EBoardElemLifeState::Alive) {
        ET_SendEvent(getEntityId(), &ETRenderNode::ET_show);
    }
}

EBoardElemLifeState GameBoardElemLogic::ET_getLifeState() const {
    return lifeState;
}

void GameBoardElemLogic::ET_setType(EBoardElemType newType) {
    type = newType;
    ET_SendEvent(getEntityId(), &ETRenderColoredTexture::ET_setTextureColor, GetElemColorForType(newType));
}

EBoardElemType GameBoardElemLogic::ET_getType() const {
    return type;
}

void GameBoardElemLogic::ET_triggerDestroy() {
    if(lifeState == EBoardElemLifeState::Destroying) {
        LogError("[GameBoardElemLogic::ET_triggerDestroy] The element already destroying");
        return;
    }
    if(ET_IsExistNode<ETBoardElemDetroyAnimation>(getEntityId())) {
        lifeState = EBoardElemLifeState::Destroying;
        ET_SendEvent(getEntityId(), &ETBoardElemDetroyAnimation::ET_playDestroy);
    } else {
        ET_onDestroyAnimEnded();
    }
}

void GameBoardElemLogic::ET_setSelected(bool flag) {
    if(flag) {
        ET_SendEvent(getEntityId(), &ETGameBoardElemSelectAnimation::ET_playSelect);
    } else {
        ET_SendEvent(getEntityId(), &ETGameBoardElemSelectAnimation::ET_playDeselect);
    }
}

void GameBoardElemLogic::ET_onDestroyAnimEnded() {
    lifeState = EBoardElemLifeState::Void;
    ET_SendEvent(getParentId(), &ETGameBoard::ET_matchElements);
    ET_SendEvent(&ETGameBoardElemDestoryEvents::ET_onElemsDestroyed, getEntityId());
}

bool GameBoardElemLogic::ET_canMatch() const {
    return moveState == EBoardElemMoveState::Static && lifeState == EBoardElemLifeState::Alive;
}
bool GameBoardElemLogic::ET_canSwitch() const {
    return moveState == EBoardElemMoveState::Static && lifeState == EBoardElemLifeState::Alive;
}