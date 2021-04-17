#include "Game/Logics/GameBoardMatchLogic.hpp"
#include "Render/ETParticlesSystem.hpp"

#include <cassert>

namespace {

EBoardElemType getSpecialElemType(EPatternType patternType) {
    EBoardElemType res = EBoardElemType::None;
    switch(patternType) {
        case EPatternType::HRocket: {
            res = EBoardElemType::HRocket;
            break;
        }
        case EPatternType::VRocket: {
            res = EBoardElemType::VRocket;
            break;
        }
        case EPatternType::Bomb: {
            res = EBoardElemType::Bomb;
            break;
        }
        case EPatternType::Star: {
            res = EBoardElemType::Star;
            break;
        }
        default: {
            assert(false && "Invalid pattern type");
        }
    }
    return res;
}

} // namespace

GameBoardMatchLogic::GameBoardMatchLogic() :
    destroyEffectScale(1.f) {
}

GameBoardMatchLogic::~GameBoardMatchLogic() {
}

void GameBoardMatchLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardMatchLogic>("GameBoardMatcher")) {
        classInfo->addField("redDestroyEffectId", &GameBoardMatchLogic::redDestroyEffectId);
        classInfo->addField("blueDestroyEffectId", &GameBoardMatchLogic::blueDestroyEffectId);
        classInfo->addField("yellowDestroyEffectId", &GameBoardMatchLogic::yellowDestroyEffectId);
        classInfo->addField("greenDestroyEffectId", &GameBoardMatchLogic::greenDestroyEffectId);
        classInfo->addField("purpleDestroyEffectId", &GameBoardMatchLogic::purpleDestroyEffectId);
        classInfo->addField("bomdDestroyEffectId", &GameBoardMatchLogic::bomdDestroyEffectId);
        classInfo->addField("destroyEffectScale", &GameBoardMatchLogic::destroyEffectScale);
    }
}

void GameBoardMatchLogic::init() {
    ETNode<ETGameBoardMatcher>::connect(getEntityId());
}

void GameBoardMatchLogic::deinit() {
}

bool GameBoardMatchLogic::ET_matchElements() {
    boardMatchState.reset();
    ET_SendEvent(&ETGameBoard::ET_readBoardMatchState, boardMatchState);

    auto patterns = FindAllMatchPatterns(boardMatchState);
    if(patterns.empty()) {
        return false;
    }

    for(auto& p : patterns) {
        matchPattern(p);
    }

    return true;
}

void GameBoardMatchLogic::matchPattern(const PatternMatch& p) {
    if(p.patternType == EPatternType::HLine || p.patternType == EPatternType::VLine || p.patternType == EPatternType::None) {
        for(auto& elem : p.points) {
            ET_SendEvent(elem->entId, &ETGameBoardElem::ET_triggerDestroy);
        }
    } else {
        ElemMergeTask mergeTask;
        mergeTask.elemType = getSpecialElemType(p.patternType);
        for(auto& elem : p.points) {
            mergeTask.elems.push_back(elem->entId);
        }
        ET_SendEvent(&ETGameBoardElemMergeManager::ET_createMergeTask, mergeTask);
    }
}

void GameBoardMatchLogic::ET_playDestroyEffect(EntityId elemId) {
    int cellSize = 0;
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

    Transform tm;
    ET_SendEventReturn(tm, elemId, &ETEntity::ET_getTransform);
    tm.scale = Vec3(static_cast<float>(cellSize) * destroyEffectScale);

    EBoardElemType elemType = EBoardElemType::None;
    ET_SendEventReturn(elemType, elemId, &ETGameBoardElem::ET_getType);

    EntityId effectId;
    switch(elemType) {
        case EBoardElemType::Red: {
            effectId = redDestroyEffectId;
            break;
        }
        case EBoardElemType::Blue: {
            effectId = blueDestroyEffectId;
            break;
        }
        case EBoardElemType::Yellow: {
            effectId = yellowDestroyEffectId;
            break;
        }
        case EBoardElemType::Purple: {
            effectId = purpleDestroyEffectId;
            break;
        }
        case EBoardElemType::Green: {
            effectId = greenDestroyEffectId;
            break;
        }
        case EBoardElemType::VRocket: {
            break;
        }
        case EBoardElemType::HRocket: {
            break;
        }
        case EBoardElemType::Bomb: {
            effectId = bomdDestroyEffectId;
            break;
        }
        case EBoardElemType::Star: {
            break;
        }
        case EBoardElemType::None: {
            [[fallthrough]];
        }
        default: {
            assert(false && "Invalid elem type");
        }
    }

    if(!effectId.isValid()) {
        return;
    }

    ET_SendEvent(effectId, &ETParticlesSystem::ET_emitWithTm, tm);
}