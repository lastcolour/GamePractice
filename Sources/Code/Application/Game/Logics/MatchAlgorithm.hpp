#ifndef __MATCH_ALGORITHM_HPP__
#define __MATCH_ALGORITHM_HPP__

#include "Game/ETGameElem.hpp"

struct BronKerboschRequest;

enum class EPatternType {
    None = 0,
    HLine,
    VLine,
    HRocket,
    VRocket,
    Bomb,
    Star
};

struct MatchPoints {
    Vec2i pt;
    EntityId entId;
    int clusterId;
    EBoardElemType elemType;
};

struct PatternMatch {
    EPatternType patternType;
    EBoardElemType elemsType;
    std::vector<const MatchPoints*> points;
};

class BoardMatchState {
public:

    BoardMatchState();
    ~BoardMatchState();

    void reset();

    void setSize(const Vec2i& newSize);
    const Vec2i& getSize() const;

    void setElem(const Vec2i& pt, EntityId elemId);
    const MatchPoints& getElem(int x, int y) const;
    MatchPoints& getElem(int x, int y);
    const MatchPoints& getElem(const Vec2i& pt) const;
    MatchPoints& getElem(const Vec2i& pt);
    int getElemIdx(int x, int y) const;

private:

    Vec2i size;
    std::vector<MatchPoints> elems;
};

std::vector<PatternMatch> FindAllMatchPatterns(BoardMatchState& board, BronKerboschRequest& cacheRequest);

#endif /* __MATCH_ALGORITHM_HPP__ */