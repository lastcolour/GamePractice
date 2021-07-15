#include "Game/Logics/GameBoardMatchLogic.hpp"

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

GameBoardMatchLogic::GameBoardMatchLogic() {
}

GameBoardMatchLogic::~GameBoardMatchLogic() {
}

void GameBoardMatchLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardMatchLogic>("GameBoardMatcher")) {
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
            ET_SendEvent(elem->entId, &ETGameBoardElem::ET_triggerDestroy, InvalidEntityId);
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