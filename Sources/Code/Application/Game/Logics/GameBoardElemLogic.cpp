#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Render/ETRenderNode.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/Logics/MatchAlgorithm.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

#include <cassert>

GameBoardElemLogic::GameBoardElemLogic() :
    state(EBoardElemState::Static),
    type(EBoardElemType::None) {
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
            {"Yellow", EBoardElemType::Yellow},
            {"HRocket", EBoardElemType::HRocket},
            {"VRocket", EBoardElemType::VRocket},
            {"Bomb", EBoardElemType::Bomb},
            {"Star", EBoardElemType::Star}
        });
    }
    if(auto classInfo = ctx.classInfo<GameBoardElemLogic>("GameBoardElem")) {
        classInfo->addField("type", &GameBoardElemLogic::type);
    }
}

void GameBoardElemLogic::init() {
    ETNode<ETGameBoardElem>::connect(getEntityId());
    ETNode<ETGameBoardRenderElem>::connect(getEntityId());
}

void GameBoardElemLogic::deinit() {
}

void GameBoardElemLogic::ET_setState(EBoardElemState newState) {
    state = newState;
}

EBoardElemState GameBoardElemLogic::ET_getState() const {
    return state;
}

void GameBoardElemLogic::ET_setType(EBoardElemType newType) {
    type = newType;
}

EBoardElemType GameBoardElemLogic::ET_getType() const {
    return type;
}

void GameBoardElemLogic::ET_triggerDestroy(EntityId sourceId) {
    assert(state == EBoardElemState::Static && "Invalid elem state");

    if(GameUtils::IsTriggerType(type)) {
        state = EBoardElemState::Triggering;
        bool applyDelay = GameUtils::ShouldApplyTriggerDelay(getEntityId(), sourceId);
        ET_SendEvent(&ETGameBoardElemTriggerManager::ET_createTriggerTask, getEntityId(), applyDelay);
    } else {
        GameUtils::PlayElemDestroyEffect(getEntityId());
        state = EBoardElemState::Destroying;
        if(ET_IsExistNode<ETBoardElemDestroyAnimation>(getEntityId())) {
            ET_SendEvent(getEntityId(), &ETBoardElemDestroyAnimation::ET_playDestroy);
        } else {
            ET_onDestroyDone();
        }
    }
}

void GameBoardElemLogic::ET_setSelected(bool flag) {
    assert((state == EBoardElemState::Static || state == EBoardElemState::Switching) && "Invalid elem state");

    if(flag) {
        ET_SendEvent(getEntityId(), &ETGameBoardElemSelectAnimation::ET_playSelect);
    } else {
        ET_SendEvent(getEntityId(), &ETGameBoardElemSelectAnimation::ET_playDeselect);
    }
}

bool GameBoardElemLogic::ET_canMatch() const {
    return state == EBoardElemState::Static;
}

bool GameBoardElemLogic::ET_canSwitch() const {
    return state == EBoardElemState::Static;
}

void GameBoardElemLogic::ET_triggerLand() {
    assert(state == EBoardElemState::Falling && "Invalid elem state");

    state = EBoardElemState::Landing;
    if(ET_IsExistNode<ETGameBoardElemLandAnimation>(getEntityId())) {
        ET_SendEvent(getEntityId(), &ETGameBoardElemLandAnimation::ET_playLand);
    } else {
        ET_onLandPlayed();
    }
}

void GameBoardElemLogic::ET_onLandPlayed() {
    assert(state == EBoardElemState::Landing && "Invalid elem state");

    state = EBoardElemState::Static;
}

void GameBoardElemLogic::ET_onDestroyDone() {
    assert(state == EBoardElemState::Destroying && "Invalid elem state");

    state = EBoardElemState::Destroyed;
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_hide);
    ET_SendEvent(&ETGameBoardElemDestoryEvents::ET_onElemsDestroyed, getEntityId());
}

void GameBoardElemLogic::ET_onMergeDone() {
    assert(state == EBoardElemState::Destroying);

    ET_onDestroyDone();
}

void GameBoardElemLogic::ET_onTriggerDone() {
    assert(state == EBoardElemState::Triggering && "Invalid elem state");
    state = EBoardElemState::Destroying;

    ET_onDestroyDone();
}

void GameBoardElemLogic::ET_initRender(UIProxyContainer& rootContainer, const Vec2& elemSize) {
    ET_SendEvent(getEntityId(), &ETRenderRect::ET_setSize, elemSize);
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_show);
    rootContainer.addItem(getEntityId(), GameUtils::BOARD_ELEM_Z_OFFSET);
}

void GameBoardElemLogic::ET_deinitRender(UIProxyContainer& rootContainer) {
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_hide);
    rootContainer.removeItem(getEntityId());
}