#include "Game/Logics/MatchAlgorithm.hpp"
#include "Game/Logics/GameBoardUtils.hpp"
#include "Game/Logics/BronKerbosch.hpp"

#include <cassert>
#include <unordered_set>

namespace {

using PatternGroupsT = std::vector<const PatternMatch*>;

PatternMatch findHLine(int x, int y, int len, const BoardMatchState& board) {
    PatternMatch p;

    auto boardSize = board.getSize();
    if(x < 0 || (x + len) > boardSize.x) {
        return p;
    }
    if(y < 0 || y >= boardSize.y) {
        return p;
    }

    auto& base = board.getElem(x, y);
    if(base.elemType == EBoardElemType::None) {
        return p;
    }

    p.points.push_back(&base);
    for(int i = 1; i < len; ++i) {
        auto& other = board.getElem(x + i, y);
        if(other.elemType != base.elemType) {
            p.points.clear();
            return p;
        } else {
            p.points.push_back(&other);
        }
    }

    p.elemsType = base.elemType;
    return p;
}

PatternMatch findVLine(int x, int y, int len, const BoardMatchState& board) {
    PatternMatch p;

    auto boardSize = board.getSize();
    if(x < 0 || x >= boardSize.x) {
        return p;
    }
    if(y < 0 || (y + len) > boardSize.y) {
        return p;
    }

    auto& base = board.getElem(x, y);
    if(base.elemType == EBoardElemType::None) {
        return p;
    }

    p.points.push_back(&base);
    for(int i = 1; i < len; ++i) {
        auto& other = board.getElem(x, y + i);
        if(other.elemType != base.elemType) {
            p.points.clear();
            return p;
        } else {
            p.points.push_back(&other);
        }
    }

    p.elemsType = base.elemType;
    return p;
}

PatternMatch findTupBomb(int x, int y, const BoardMatchState& board) {
    auto p1 = findHLine(x, y, 3, board);
    if(p1.points.empty()) {
        return p1;
    }
    auto p2 = findVLine(x + 1, y - 2, 3, board);
    if(p2.points.empty()) {
        return p2;
    }
    p1.points.push_back(p2.points[0]);
    p1.points.push_back(p2.points[1]);
    return p1;
}

PatternMatch findTLeftBomb(int x, int y, const BoardMatchState& board) {
    auto p1 = findHLine(x, y, 3, board);
    if(p1.points.empty()) {
        return p1;
    }
    auto p2 = findVLine(x, y - 1, 3, board);
    if(p2.points.empty()) {
        return p2;
    }
    p1.points.push_back(p2.points[0]);
    p1.points.push_back(p2.points[2]);
    return p1;
}

PatternMatch findTRightBomb(int x, int y, const BoardMatchState& board) {
    auto p1 = findHLine(x, y, 3, board);
    if(p1.points.empty()) {
        return p1;
    }
    auto p2 = findVLine(x + 2, y - 1, 3, board);
    if(p2.points.empty()) {
        return p2;
    }
    p1.points.push_back(p2.points[0]);
    p1.points.push_back(p2.points[2]);
    return p1;
}

PatternMatch findTdownBomb(int x, int y, const BoardMatchState& board) {
    auto p1 = findHLine(x, y, 3, board);
    if(p1.points.empty()) {
        return p1;
    }
    auto p2 = findVLine(x + 1, y, 3, board);
    if(p2.points.empty()) {
        return p2;
    }
    p1.points.push_back(p2.points[1]);
    p1.points.push_back(p2.points[2]);
    return p1;
}

PatternMatch findLleftUpBomb(int x, int y, const BoardMatchState& board) {
    auto p1 = findHLine(x, y, 3, board);
    if(p1.points.empty()) {
        return p1;
    }
    auto p2 = findVLine(x, y - 2, 3, board);
    if(p2.points.empty()) {
        return p2;
    }
    p1.points.push_back(p2.points[0]);
    p1.points.push_back(p2.points[1]);
    return p1;
}

PatternMatch findLrightUpBomb(int x, int y, const BoardMatchState& board) {
    auto p1 = findHLine(x, y, 3, board);
    if(p1.points.empty()) {
        return p1;
    }
    auto p2 = findVLine(x + 2, y - 2, 3, board);
    if(p2.points.empty()) {
        return p2;
    }
    p1.points.push_back(p2.points[0]);
    p1.points.push_back(p2.points[1]);
    return p1;
}

PatternMatch findLleftDownBomb(int x, int y, const BoardMatchState& board) {
    auto p1 = findHLine(x, y, 3, board);
    if(p1.points.empty()) {
        return p1;
    }
    auto p2 = findVLine(x, y, 3, board);
    if(p2.points.empty()) {
        return p2;
    }
    p1.points.push_back(p2.points[1]);
    p1.points.push_back(p2.points[2]);
    return p1;
}

PatternMatch findLrightDownBomb(int x, int y, const BoardMatchState& board) {
    auto p1 = findHLine(x, y, 3, board);
    if(p1.points.empty()) {
        return p1;
    }
    auto p2 = findVLine(x + 2, y, 3, board);
    if(p2.points.empty()) {
        return p2;
    }
    p1.points.push_back(p2.points[1]);
    p1.points.push_back(p2.points[2]);
    return p1;
}

int calcPatternScore(const PatternMatch& p) {
    int score = 0;
    switch(p.patternType) {
        case EPatternType::HLine: {
            score = 1;
            break;
        }
        case EPatternType::VLine: {
            score = 1;
            break;
        }
        case EPatternType::HRocket: {
            score = 10;
            break;
        }
        case EPatternType::VRocket: {
            score = 10;
            break;
        }
        case EPatternType::Bomb: {
            score = 100;
            break;
        }
        case EPatternType::Star: {
            score = 1000;
            break;
        }
        default: {
            assert(false && "Invalid pattern type");
        }
    }
    return score;
}

bool patternsIntersect(const PatternMatch& p1, const PatternMatch& p2) {
    if(p1.elemsType != p2.elemsType) {
        return false;
    }
    if(p1.points[0]->clusterId != p2.points[0]->clusterId) {
        return false;
    }
    for(auto& e1 : p1.points) {
        for(auto& e2 : p2.points) {
            if(e1 == e2) {
                return true;
            }
        }
    }
    return false;
}

std::vector<PatternMatch> mergeAllMatchPatterns(const std::vector<PatternMatch>& patterns, BronKerboschRequest& cacheRequest) {
    int n = static_cast<int>(patterns.size());

    cacheRequest.adjMat.reset(n);
    cacheRequest.adjMat.reset(n);

    for(int i = 0; i < (n - 1); ++i) {
        cacheRequest.adjMat.setVertexCost(i, calcPatternScore(patterns[i]));
        for(int j = i + 1; j < n; ++j) {
            bool hasIntersect = patternsIntersect(patterns[i], patterns[j]);
            if(!hasIntersect) {
                cacheRequest.adjMat.addEdge(i, j);
            }
        }
    }

    cacheRequest.adjMat.setVertexCost(n - 1, calcPatternScore(patterns[n - 1]));

    BronKerbosch(cacheRequest);

    std::unordered_set<const MatchPoints*> remainingPoints;
    for(auto& p : patterns) {
        for(auto& elem : p.points) {
            remainingPoints.insert(elem);
        }
    }

    std::vector<PatternMatch> result;

    for(auto& i : cacheRequest.result) {
        auto& p = patterns[i];
        for(auto& elem : p.points) {
            auto it = remainingPoints.find(elem);
            if(it != remainingPoints.end()) {
                remainingPoints.erase(it);
            }
        }
        result.push_back(p);
    }

    if(!remainingPoints.empty()) {

        PatternMatch nonePattern;
        nonePattern.patternType = EPatternType::None;
        nonePattern.elemsType = EBoardElemType::None;
        nonePattern.points.insert(nonePattern.points.begin(),
            remainingPoints.begin(), remainingPoints.end());

        result.push_back(nonePattern);
    }

    return result;
}

bool isInsideBoard(const Vec2i& size, const Vec2i& pt) {
    return pt < size && pt >= Vec2i(0);
}

void findAllClusters(BoardMatchState& board) {
    auto size = board.getSize();
    std::vector<int> visited(size.x * size.y, false);
    std::vector<Vec2i> visitQueue;
    int currClusterId = 0;
    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            auto idx = board.getElemIdx(i, j);
            if(visited[idx]) {
                continue;
            }
            auto clusterElemsType = board.getElem(i, j).elemType;
            visitQueue.push_back(Vec2i(i, j));
            while(!visitQueue.empty()) {
                Vec2i pt = visitQueue.back();
                visitQueue.pop_back();

                board.getElem(pt).clusterId = currClusterId;
                visited[board.getElemIdx(pt.x, pt.y)] = true;

                {
                    Vec2i left(pt.x - 1, pt.y);
                    if(isInsideBoard(size, left) && board.getElem(left).elemType == clusterElemsType) {
                        if(!visited[board.getElemIdx(left.x, left.y)]) {
                            visitQueue.push_back(left);
                        }
                    }
                }
                {
                    Vec2i right(pt.x + 1, pt.y);
                    if(isInsideBoard(size, right) && board.getElem(right).elemType == clusterElemsType) {
                        if(!visited[board.getElemIdx(right.x, right.y)]) {
                            visitQueue.push_back(right);
                        }
                    }
                }
                {
                    Vec2i top(pt.x, pt.y + 1);
                    if(isInsideBoard(size, top) && board.getElem(top).elemType == clusterElemsType) {
                        if(!visited[board.getElemIdx(top.x, top.y)]) {
                            visitQueue.push_back(top);
                        }
                    }
                }
                {
                    Vec2i bot(pt.x, pt.y - 1);
                    if(isInsideBoard(size, bot) && board.getElem(bot).elemType == clusterElemsType) {
                        if(!visited[board.getElemIdx(bot.x, bot.y)]) {
                            visitQueue.push_back(bot);
                        }
                    }
                }
            }
            ++currClusterId;
        }
    }
}

} // namespace

BoardMatchState::BoardMatchState() :
    size(0) {
}

BoardMatchState::~BoardMatchState() {
}

void BoardMatchState::reset() {
    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            auto& elem = getElem(i, j);
            elem.elemType = EBoardElemType::None;
            elem.entId = InvalidEntityId;
            elem.clusterId = -1;
            elem.pt = Vec2i(i, j);
        }
    }
}

void BoardMatchState::setSize(const Vec2i& newSize) {
    size = newSize;
    elems.resize(newSize.x * newSize.y);
    reset();
}

const Vec2i& BoardMatchState::getSize() const {
    return size;
}

void BoardMatchState::setElem(const Vec2i& pt, EntityId entId) {
    auto& elem = getElem(pt.x, pt.y);
    elem.entId = entId;
    if(!elem.entId.isValid()) {
        elem.elemType = EBoardElemType::None;
    } else {
        elem.elemType = GameUtils::GetElemType(elem.entId);
    }
}

MatchPoints& BoardMatchState::getElem(int x, int y) {
    return elems[getElemIdx(x, y)];
}

const MatchPoints& BoardMatchState::getElem(int x, int y) const {
    return elems[getElemIdx(x, y)];
}

const MatchPoints& BoardMatchState::getElem(const Vec2i& pt) const {
    return getElem(pt.x, pt.y);
}

MatchPoints& BoardMatchState::getElem(const Vec2i& pt) {
    return getElem(pt.x, pt.y);
}

int BoardMatchState::getElemIdx(int x, int y) const {
    return x + size.x * y;
}

std::vector<PatternMatch> FindAllMatchPatterns(BoardMatchState& board, BronKerboschRequest& cacheRequest) {
    findAllClusters(board);

    std::vector<PatternMatch> allMatches;
    auto boardSize = board.getSize();
    for(int i = 0; i < boardSize.x; ++i) {
        for(int j = 0; j < boardSize.y; ++j) {
            bool hasSimpleLine = false;
            {
                auto p = findHLine(i, j, 3, board);
                if(!p.points.empty()) {
                    hasSimpleLine = true;
                    p.patternType = EPatternType::HLine;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findVLine(i, j, 3, board);
                if(!p.points.empty()) {
                    hasSimpleLine = true;
                    p.patternType = EPatternType::VLine;
                    allMatches.emplace_back(p);
                }
            }
            if(!hasSimpleLine) {
                continue;
            }
            {
                auto p = findHLine(i, j, 4, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::HRocket;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findVLine(i, j, 4, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::VRocket;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findTupBomb(i, j, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Bomb;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findTdownBomb(i, j, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Bomb;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findTLeftBomb(i, j, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Bomb;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findTRightBomb(i, j, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Bomb;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findLleftUpBomb(i, j, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Bomb;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findLrightUpBomb(i, j, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Bomb;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findLleftDownBomb(i, j, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Bomb;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findLrightDownBomb(i, j, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Bomb;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findHLine(i, j, 5, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Star;
                    allMatches.emplace_back(p);
                }
            }
            {
                auto p = findVLine(i, j, 5, board);
                if(!p.points.empty()) {
                    p.patternType = EPatternType::Star;
                    allMatches.emplace_back(p);
                }
            }
        }
    }

    if(allMatches.empty()) {
        return allMatches;
    }

    return mergeAllMatchPatterns(allMatches, cacheRequest);
}