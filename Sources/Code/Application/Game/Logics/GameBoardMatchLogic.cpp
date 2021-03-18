#include "Game/Logics/GameBoardMatchLogic.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "Render/ETRenderNode.hpp"

#include <cassert>

namespace {

const int DESTROY_EFFECT_Z_INDEX = 3;

} // namespace

GameBoardMatchLogic::GameBoardMatchLogic() :
    destroyEffectScale(1.f),
    minLineLen(3),
    maxLineLen(5) {
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

    uiProxies.addItem(redDestroyEffectId, DESTROY_EFFECT_Z_INDEX);
    uiProxies.addItem(blueDestroyEffectId, DESTROY_EFFECT_Z_INDEX);
    uiProxies.addItem(yellowDestroyEffectId, DESTROY_EFFECT_Z_INDEX);
    uiProxies.addItem(greenDestroyEffectId, DESTROY_EFFECT_Z_INDEX);
    uiProxies.addItem(purpleDestroyEffectId, DESTROY_EFFECT_Z_INDEX);
    uiProxies.setUIParent(getEntityId());
}

void GameBoardMatchLogic::deinit() {
}

bool GameBoardMatchLogic::isElementsMatch(EntityId firstId, EntityId secondId) const {
    {
        bool firstCanMatch = false;
        ET_SendEventReturn(firstCanMatch, firstId, &ETGameBoardElem::ET_canMatch);
        if(!firstCanMatch) {
            return false;
        }

        bool secondCanMatch = false;
        ET_SendEventReturn(secondCanMatch, secondId, &ETGameBoardElem::ET_canMatch);
        if(!secondCanMatch) {
            return false;
        }
    }
    {
        EBoardElemType firstType = EBoardElemType::None;
        ET_SendEventReturn(firstType, firstId, &ETGameBoardElem::ET_getType);

        EBoardElemType secondType = EBoardElemType::None;
        ET_SendEventReturn(secondType, secondId, &ETGameBoardElem::ET_getType);

        if(secondType != firstType || secondType == EBoardElemType::None) {
            return false;
        }
    }

    return true;
}

bool GameBoardMatchLogic::findMatchLine(const Vec2i& startPt, int lineLen, bool isHorizontal, MatchElemCollectionT& result) {
    EntityId firstElemId = InvalidEntityId;
    ET_SendEventReturn(firstElemId, &ETGameBoard::ET_getElemByBoardPos, startPt);
    if(!firstElemId.isValid()) {
        return false;
    }
    std::vector<EntityId> lineElems;
    lineElems.reserve(lineLen);
    lineElems.push_back(firstElemId);
    for(int i = 1; i < lineLen; ++i) {
        EntityId nextElemId = InvalidEntityId;
        Vec2i nextPt = startPt;
        if(isHorizontal) {
            nextPt.x += i;
        } else {
            nextPt.y += i;
        }
        ET_SendEventReturn(nextElemId, &ETGameBoard::ET_getElemByBoardPos, nextPt);
        if(!nextElemId.isValid()) {
            return false;
        }
        if(!isElementsMatch(firstElemId, nextElemId)) {
            return false;
        }
        lineElems.push_back(nextElemId);
    }
    result.insert(lineElems.begin(), lineElems.end());
    return true;
}

void GameBoardMatchLogic::ET_destoryMatchedElems() {
    Vec2i boardSize(0);
    ET_SendEventReturn(boardSize, &ETGameBoard::ET_getBoardSize);

    MatchElemCollectionT mathElems;
    for(int i = 0; i < boardSize.x;  ++i) {
        for(int j = 0; j < boardSize.y; ++j) {
            for(int k = maxLineLen; k >= minLineLen; --k) {
                if(findMatchLine(Vec2i(i, j), k, true, mathElems)) {
                    break;
                }
            }
            for(int k = maxLineLen; k >= minLineLen; --k) {
                if(findMatchLine(Vec2i(i, j), k, false, mathElems)) {
                    break;
                }
            }
        }
    }

    for(auto& elemId : mathElems) {
        ET_SendEvent(elemId, &ETGameBoardElem::ET_triggerDestroy);

        int cellSize = 0;
        ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

        Transform tm;
        ET_SendEventReturn(tm, elemId, &ETEntity::ET_getTransform);

        tm.scale = Vec3(cellSize * destroyEffectScale);

        triggerDestroyEffect(elemId, tm);
    }
}

void GameBoardMatchLogic::triggerDestroyEffect(EntityId elemId, const Transform& emiTm) {
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

    ET_SendEvent(effectId, &ETParticlesSystem::ET_emitWithTm, emiTm);
}