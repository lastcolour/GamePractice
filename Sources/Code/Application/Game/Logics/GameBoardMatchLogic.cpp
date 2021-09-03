#include "Game/Logics/GameBoardMatchLogic.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

#include <cassert>

namespace {

const int MAX_BRON_KERBOSCH_ITER = 500000;
const float LOG_MS_THRESHOLD_MS = 1.f;

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

bool makeSwappedElemFirst(EntityId swappedId, PatternMatch& p) {
    for(size_t i = 0, sz = p.points.size(); i < sz; ++i) {
        if(p.points[i]->entId == swappedId) {
            std::swap(p.points[0], p.points[i]);
            return true;
        }
    }
    return false;
}

} // namespace

GameBoardMatchLogic::GameBoardMatchLogic() {
}

GameBoardMatchLogic::~GameBoardMatchLogic() {
}

void GameBoardMatchLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardMatchLogic>("GameBoardMatcher")) {
        (void)classInfo;
    }
}

void GameBoardMatchLogic::init() {
    ETNode<ETGameBoardMatcher>::connect(getEntityId());
    ETNode<ETGameBoardSpawnerEvents>::connect(getEntityId());

    cacheRequest.stopIterCount = MAX_BRON_KERBOSCH_ITER;
}

void GameBoardMatchLogic::deinit() {
}

bool GameBoardMatchLogic::ET_matchElements() {
    boardMatchState.reset();
    ET_SendEvent(&ETGameBoard::ET_readBoardMatchState, boardMatchState);

    auto patterns = FindAllMatchPatterns(boardMatchState, cacheRequest);

    if(cacheRequest.duration > LOG_MS_THRESHOLD_MS) {
        LogInfo("[GameBoardMatchLogic::ET_matchElements] Finish match: 'iter'=%d, 'maxDepth'=%d, 'duration'=%.2fms",
            cacheRequest.iterCount,
            cacheRequest.maxDepth,
            cacheRequest.duration);
    }

    if(patterns.empty()) {
        return false;
    }

    if(lastSwappedElem.isValid()) {
        for(auto& p : patterns) {
            if(makeSwappedElemFirst(lastSwappedElem, p)) {
                break;
            }
        }
        lastSwappedElem = InvalidEntityId;
    }

    for(auto& p : patterns) {
        matchPattern(p);
    }

    return true;
}

void GameBoardMatchLogic::ET_setLastSwappedElem(EntityId elemId) {
    lastSwappedElem = elemId;
}

void GameBoardMatchLogic::ET_onStartLoading() {
    lastSwappedElem = InvalidEntityId;
}

void GameBoardMatchLogic::ET_onStartDestroying() {
}

void GameBoardMatchLogic::matchPattern(const PatternMatch& p) {
    if(p.patternType == EPatternType::HLine || p.patternType == EPatternType::VLine || p.patternType == EPatternType::None) {
        for(auto& elem : p.points) {
            GameUtils::TryTriggerElemDestroy(InvalidEntityId, elem->entId);
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