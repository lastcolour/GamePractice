#include "Game/Logics/MatchAlgorithm.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

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

int calcGroupScore(const PatternGroupsT& group) {
    int totalScore = 0;
    for(auto& p : group) {
        totalScore += calcPatternScore(*p);
    }
    return totalScore;
}

bool canMergePatterns(const PatternMatch& p1, const PatternMatch& p2) {
    if(p1.elemsType != p2.elemsType) {
        return false;
    }
    for(auto& e1 : p1.points) {
        for(auto& e2 : p2.points) {
            if(e1 == e2) {
                return false;
            }
        }
    }
    return true;
}

PatternGroupsT tryMergeToGroup(const PatternMatch& p, PatternGroupsT& group) {
    std::vector<const PatternMatch*> res;
    for(auto& otherP : group) {
        if(!canMergePatterns(p, *otherP)) {
            return res;
        }
    }
    res = group;
    res.push_back(&p);
    return res;
}

std::vector<PatternMatch> mergeAllMatchPatterns(const std::vector<PatternMatch>& patterns) {
    std::vector<PatternGroupsT> allGroups;

    for(auto& p : patterns) {
        PatternGroupsT group;
        group.emplace_back(&p);
        allGroups.emplace_back(std::move(group));
    }

    std::vector<PatternGroupsT> currGroups = allGroups;
    std::vector<PatternGroupsT> newGroups;

    while(!currGroups.empty()) {
        for(auto& p : patterns) {
            for(auto& group : currGroups) {
                auto newGroup = tryMergeToGroup(p, group);
                if(!newGroup.empty()) {
                    newGroups.push_back(newGroup);
                    allGroups.push_back(std::move(newGroup));
                }
            }
        }
        currGroups = std::move(newGroups);
    }

    auto bestGroup = &allGroups[0];
    auto bestScore = calcGroupScore(*bestGroup);

    for(size_t i = 1, sz = allGroups.size(); i < sz; ++i) {
        auto& group = allGroups[i];
        auto score = calcGroupScore(group);
        if(score > bestScore) {
            bestGroup = &group;
            bestScore = score;
        }
    }

    std::unordered_set<const MatchPoints*> remainingPoints;
    for(auto& p : patterns) {
        for(auto& elem : p.points) {
            remainingPoints.insert(elem);
        }
    }

    std::vector<PatternMatch> resPatterns;

    for(auto& p : *bestGroup) {
        resPatterns.push_back(*p);
        for(auto& elem : p->points) {
            auto it = remainingPoints.find(elem);
            if(it != remainingPoints.end()) {
                remainingPoints.erase(it);
            }
        }
    }

    if(!remainingPoints.empty()) {
        PatternMatch p;
        p.patternType = EPatternType::None;
        p.elemsType = EBoardElemType::None;
        for(auto& elem : remainingPoints) {
            p.points.push_back(elem);
        }
        resPatterns.push_back(p);
    }

    return resPatterns;
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
    auto i = x + size.x * y;
    return elems[i];
}

const MatchPoints& BoardMatchState::getElem(int x, int y) const {
    auto i = x + size.x * y;
    return elems[i];
}

std::vector<PatternMatch> FindAllMatchPatterns(const BoardMatchState& board) {
    auto boardSize = board.getSize();

    std::vector<PatternMatch> allMatches;

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

    return mergeAllMatchPatterns(allMatches);
}