#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Render/ETRenderNode.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/Logics/MatchAlgorithm.hpp"

#include <cassert>

GameBoardElemLogic::GameBoardElemLogic() :
    state(EBoardElemState::Static),
    type(EBoardElemType::Yellow),
    mutateTo(EPatternType::None) {
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
}

void GameBoardElemLogic::deinit() {
}

void GameBoardElemLogic::ET_setElemState(EBoardElemState newState) {
    state = newState;
}

EBoardElemState GameBoardElemLogic::ET_getState() const {
    return state;
}

EBoardElemType GameBoardElemLogic::ET_getType() const {
    return type;
}

void GameBoardElemLogic::ET_triggerDestroy() {
    assert(state == EBoardElemState::Static && "Invalid elem state");

    state = EBoardElemState::Destroying;
    if(ET_IsExistNode<ETBoardElemDestroyAnimation>(getEntityId())) {
        ET_SendEvent(getEntityId(), &ETBoardElemDestroyAnimation::ET_playDestroy);
    } else {
        ET_onDestroyPlayed();
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

void GameBoardElemLogic::ET_onDestroyPlayed() {
    assert(state == EBoardElemState::Destroying && "Invalid elem state");

    state = EBoardElemState::Destroyed;
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_hide);
    ET_SendEvent(&ETGameBoardElemDestoryEvents::ET_onElemsDestroyed, getEntityId());
}

void GameBoardElemLogic::ET_setMutateAfterMerge(EPatternType pattern) {
    assert(state == EBoardElemState::Static && "Invalid elem state");

    state = EBoardElemState::Mutating;
    mutateTo = pattern;
}

void GameBoardElemLogic::ET_triggerMutate() {
    assert(state == EBoardElemState::Mutating && "Invalid elem state");

    state = EBoardElemState::Destroying;
    ET_onDestroyPlayed();

    ET_SendEvent(&ETGameBoard::ET_replaceElemToSpecial, getEntityId(), EBoardElemType::HRocket);
    mutateTo = EPatternType::None;
}

void GameBoardElemLogic::ET_triggerMergeTo(EntityId mergeTargetId) {
    assert(state == EBoardElemState::Static && "Invalid elem state");

    state = EBoardElemState::Merging;
    ET_SendEvent(&ETGameBoardElemMergeManager::ET_createMergeTask,
        getEntityId(), mergeTargetId);
}

void GameBoardElemLogic::ET_onMergeDone(EntityId elemId) {
    if(state == EBoardElemState::Merging) {
        state = EBoardElemState::Destroying;
        ET_onDestroyPlayed();
    } else {
        assert(false && "Invalid elem state");
    }
}