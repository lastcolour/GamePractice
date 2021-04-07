#include "Game/Logics/GameBoardMatchLogic.hpp"
#include "Render/ETParticlesSystem.hpp"

#include <cassert>

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
        classInfo->addField("destroyEffectScale", &GameBoardMatchLogic::destroyEffectScale);
    }
}

void GameBoardMatchLogic::init() {
    ETNode<ETGameBoardMatcher>::connect(getEntityId());
}

void GameBoardMatchLogic::deinit() {
}

bool GameBoardMatchLogic::ET_matchElements() {
    int cellSize = 0;
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

    boardMatchState.reset();
    ET_SendEvent(&ETGameBoard::ET_readBoardMatchState, boardMatchState);

    auto patterns = FindAllMatchPatterns(boardMatchState);
    if(patterns.empty()) {
        return false;
    }

    for(auto& p : patterns) {
        matchPattern(p, cellSize);
    }

    return true;
}

void GameBoardMatchLogic::matchPattern(const PatternMatch& p, int cellSize) {
    if(p.patternType == EPatternType::HLine || p.patternType == EPatternType::VLine || p.patternType == EPatternType::None) {
        for(auto& elem : p.points) {
            ET_SendEvent(elem->entId, &ETGameBoardElem::ET_triggerDestroy);
            playDestroyEffect(elem->entId, cellSize);
        }
    } else {
        auto midElem = p.points[1];
        for(auto& elem : p.points) {
            if(elem != midElem) {
                ET_SendEvent(elem->entId, &ETGameBoardElem::ET_triggerMergeTo, midElem->entId);
            }
        }
        ET_SendEvent(midElem->entId, &ETGameBoardElem::ET_setMutateAfterMerge, p.patternType);
    }
}

void GameBoardMatchLogic::playDestroyEffect(EntityId elemId, int cellSize) {
    Transform tm;
    ET_SendEventReturn(tm, elemId, &ETEntity::ET_getTransform);
    tm.scale = Vec3(cellSize * destroyEffectScale);

    EBoardElemType elemType = EBoardElemType::None;
    ET_SendEventReturn(elemType, elemId, &ETGameBoardElem::ET_getType);

    Vec2i elemCellPt(-1);
    ET_SendEventReturn(elemCellPt, &ETGameBoard::ET_getElemBoardPos, elemId);
    ET_SendEvent(&ETGameBoardElemHighlighter::ET_highlightCell, elemCellPt);

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