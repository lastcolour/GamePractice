#include "Game/Logics/GameBoardMatchLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGameElem.hpp"

GameBoardMatchLogic::GameBoardMatchLogic() :
    minLineLen(3),
    maxLineLen(5) {
}

GameBoardMatchLogic::~GameBoardMatchLogic() {
}

void GameBoardMatchLogic::Reflect(ReflectContext& ctx) {
    ctx.classInfo<GameBoardMatchLogic>("GameBoardMatcher");
}

bool GameBoardMatchLogic::init() {
    ETNode<ETGameBoardMatcher>::connect(getEntityId());
    return true;
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
    ET_SendEventReturn(firstElemId, getEntityId(), &ETGameBoard::ET_getElemByBoardPos, startPt);
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
        ET_SendEventReturn(nextElemId, getEntityId(), &ETGameBoard::ET_getElemByBoardPos, nextPt);
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

std::vector<EntityId> GameBoardMatchLogic::ET_getMatchedElements() {
    Vec2i boardSize(0);
    ET_SendEventReturn(boardSize, getEntityId(), &ETGameBoard::ET_getBoardSize);

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

    std::vector<EntityId> result(mathElems.begin(), mathElems.end());
    return result;
}