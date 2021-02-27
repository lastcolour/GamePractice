#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Render/ETRenderNode.hpp"
#include "Core/ETLogger.hpp"
#include "Game/ETGameInterfaces.hpp"

GameBoardElemLogic::GameBoardElemLogic() :
    lifeState(EBoardElemLifeState::Void),
    moveState(EBoardElemMoveState::Static),
    type(EBoardElemType::Yellow) {
}

GameBoardElemLogic::~GameBoardElemLogic() {
}

void GameBoardElemLogic::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EBoardElemType>("EBoardElemType")) {
        enumInfo->addValues<EBoardElemType>({
            {"Red", EBoardElemType::Red},
            {"Blue", EBoardElemType::Blue},
            {"Green", EBoardElemType::Green},
            {"Purple", EBoardElemType::Purple},
            {"Yellow", EBoardElemType::Yellow}
        });
    }
    if(auto classInfo = ctx.classInfo<GameBoardElemLogic>("GameBoardElem")) {
        classInfo->addField("type", &GameBoardElemLogic::type);
    }
}

void GameBoardElemLogic::init() {
    ETNode<ETGameBoardElem>::connect(getEntityId());
    ETNode<ETBoardElemDestroyAnimationEvents>::connect(getEntityId());
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

EBoardElemType GameBoardElemLogic::ET_getType() const {
    return type;
}

void GameBoardElemLogic::ET_triggerDestroy() {
    if(lifeState == EBoardElemLifeState::Destroying) {
        LogError("[GameBoardElemLogic::ET_triggerDestroy] The element already destroying");
        return;
    }
    if(ET_IsExistNode<ETBoardElemDestroyAnimation>(getEntityId())) {
        lifeState = EBoardElemLifeState::Destroying;
        ET_SendEvent(getEntityId(), &ETBoardElemDestroyAnimation::ET_playDestroy);
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
    ET_SendEvent(&ETGameBoard::ET_matchElements);
    ET_SendEvent(&ETGameBoardElemDestoryEvents::ET_onElemsDestroyed, getEntityId());
}

bool GameBoardElemLogic::ET_canMatch() const {
    return moveState == EBoardElemMoveState::Static && lifeState == EBoardElemLifeState::Alive;
}
bool GameBoardElemLogic::ET_canSwitch() const {
    return moveState == EBoardElemMoveState::Static && lifeState == EBoardElemLifeState::Alive;
}