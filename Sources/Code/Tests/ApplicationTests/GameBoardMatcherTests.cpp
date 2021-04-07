#include "GameBoardMatcherTests.hpp"
#include "Game/Logics/MatchAlgorithm.hpp"

namespace {

BoardMatchState CreateBoardState(const std::vector<std::string>& setup) {
    Vec2i size(0);
    size.y = static_cast<int>(setup.size());
    size.x = static_cast<int>(setup[0].size());

    BoardMatchState state;
    state.setSize(size);

    for(int i = 0; i < size.x; ++i) {
        for(int j = 0; j < size.y; ++j) {
            Vec2i boardPt(i, j);
            auto ch = setup[setup.size() - j - 1][i];
            if(ch == 'S') {
                state.getElem(i, j).elemType = EBoardElemType::Blue;
            } else {
                state.getElem(i, j).elemType = EBoardElemType::None;
            }
        }
    }

    return state;
}

} // namespace

TEST_F(GameBoardMatcherTests, TestOnly3HLine) {
    auto boardState = CreateBoardState({
        "SSS",
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::HLine);
    EXPECT_EQ(patterns[0].points.size(), 3);
}

TEST_F(GameBoardMatcherTests, TestOnly3VLine) {
    auto boardState = CreateBoardState({
        "S",
        "S",
        "S"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::VLine);
    EXPECT_EQ(patterns[0].points.size(), 3);
}

TEST_F(GameBoardMatcherTests, Test3VLine) {
    auto boardState = CreateBoardState({
        "DDD",
        "DSD",
        "DSD",
        "DSD",
        "DDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::VLine);
    EXPECT_EQ(patterns[0].points.size(), 3);
}

TEST_F(GameBoardMatcherTests, Test3HLine) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DSSSD",
        "DDDDD",
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::HLine);
    EXPECT_EQ(patterns[0].points.size(), 3);
}

TEST_F(GameBoardMatcherTests, Test4HLine) {
    auto boardState = CreateBoardState({
        "DDDDDD",
        "DSSSSD",
        "DDDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::HRocket);
    EXPECT_EQ(patterns[0].points.size(), 4);
}

TEST_F(GameBoardMatcherTests, Test4VLine) {
    auto boardState = CreateBoardState({
        "DDD",
        "DSD",
        "DSD",
        "DSD",
        "DSD",
        "DDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::VRocket);
    EXPECT_EQ(patterns[0].points.size(), 4);
}

TEST_F(GameBoardMatcherTests, Test5HLine) {
    auto boardState = CreateBoardState({
        "DDDDDDD",
        "DSSSSSD",
        "DDDDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Star);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, Test5VLine) {
    auto boardState = CreateBoardState({
        "DDD",
        "DSD",
        "DSD",
        "DSD",
        "DSD",
        "DSD",
        "DDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Star);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, Test3MidCrossLine) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DDSDD",
        "DSSSD",
        "DDSDD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 2);
    EXPECT_EQ(patterns[0].patternType, EPatternType::HLine);
    EXPECT_EQ(patterns[0].points.size(), 3);

    EXPECT_EQ(patterns[1].patternType, EPatternType::None);
    EXPECT_EQ(patterns[1].points.size(), 2);
}

TEST_F(GameBoardMatcherTests, Test3MidTDownLine) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DSSSD",
        "DDSDD",
        "DDSDD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, Test3MidTUpLine) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DDSDD",
        "DDSDD",
        "DSSSD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, Test3LeftLUpLine) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DSDDD",
        "DSDDD",
        "DSSSD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, Test3RightLUpLine) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DDDSD",
        "DDDSD",
        "DSSSD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, Test3LeftLDownLine) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DSSSD",
        "DSDDD",
        "DSDDD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, Test3RightLDownLine) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DSSSD",
        "DDDSD",
        "DDDSD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, TestLeftT) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DSDDD",
        "DSSSD",
        "DSDDD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, TestRightT) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DDDSD",
        "DSSSD",
        "DDDSD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 1);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);
}

TEST_F(GameBoardMatcherTests, Test3x3Quad) {
    auto boardState = CreateBoardState({
        "DDDDD",
        "DSSSD",
        "DSSSD",
        "DSSSD",
        "DDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 2);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);

    EXPECT_EQ(patterns[1].patternType, EPatternType::None);
    EXPECT_EQ(patterns[1].points.size(), 4);
}

TEST_F(GameBoardMatcherTests, Test3x4Quad) {
    auto boardState = CreateBoardState({
        "DDDDDD",
        "DSSSSD",
        "DSSSSD",
        "DSSSSD",
        "DDDDDD"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 3);
    EXPECT_EQ(patterns[0].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[0].points.size(), 5);

    EXPECT_EQ(patterns[1].patternType, EPatternType::Bomb);
    EXPECT_EQ(patterns[1].points.size(), 5);

    EXPECT_EQ(patterns[2].patternType, EPatternType::None);
    EXPECT_EQ(patterns[2].points.size(), 2);
}

/*

TEST_F(GameBoardMatcherTests, Test10x10Quad) {
    auto boardState = CreateBoardState({
        "SSSSSSSSSS",
        "SSSSSSSSSS",
        "SSSSSSSSSS",
        "SSSSSSSSSS",
        "SSSSSSSSSS",
        "SSSSSSSSSS",
        "SSSSSSSSSS",
        "SSSSSSSSSS",
        "SSSSSSSSSS",
        "SSSSSSSSSS"
    });

    auto patterns = FindAllMatchPatterns(boardState);

    ASSERT_EQ(patterns.size(), 20);
    for(int i = 0; i < 20; ++i) {
        EXPECT_EQ(patterns[i].patternType, EPatternType::Star);
        EXPECT_EQ(patterns[i].points.size(), 5);
    }
}

*/