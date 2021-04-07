#include "GameBoardFSMTests.hpp"
#include "Game/Logics/GameBoardFSM.hpp"

TEST_F(GameBoardFSMTests, CheckMovingState) {
    GameBoardFSM gameBoardFSM;

    auto& state = gameBoardFSM.getState();
    state.hasMovingElems = true;

    EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
    auto successed = gameBoardFSM.queryPass(pass);

    EXPECT_TRUE(successed);
    EXPECT_EQ(pass, EGameBoardUpdatePass::Move);

    successed = gameBoardFSM.querySubPass(pass);
    EXPECT_FALSE(successed);
}

TEST_F(GameBoardFSMTests, CheckMatchAfterMoveDone) {
    GameBoardFSM gameBoardFSM;

    auto& state = gameBoardFSM.getState();
    state.hasMovingElems = true;

    EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
    auto successed = gameBoardFSM.queryPass(pass);

    EXPECT_TRUE(successed);
    EXPECT_EQ(pass, EGameBoardUpdatePass::Move);

    state.hasMovingElems = false;
    state.isMatchRequested = true;

    successed = gameBoardFSM.querySubPass(pass);
    EXPECT_TRUE(successed);
    EXPECT_EQ(pass, EGameBoardUpdatePass::Match);

    state.isMatchRequested = false;

    successed = gameBoardFSM.querySubPass(pass);
    EXPECT_FALSE(successed);
}

TEST_F(GameBoardFSMTests, CheckMatchAfterMerge) {
    GameBoardFSM gameBoardFSM;

    auto& state = gameBoardFSM.getState();
    state.hasMergingElems = true;

    EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
    auto successed = gameBoardFSM.queryPass(pass);

    EXPECT_TRUE(successed);
    EXPECT_EQ(pass, EGameBoardUpdatePass::Merge);

    successed = gameBoardFSM.querySubPass(pass);
    EXPECT_FALSE(successed);

    state.isMatchRequested = true;
    state.hasMergingElems = false;

    pass = EGameBoardUpdatePass::Static;
    successed = gameBoardFSM.queryPass(pass);

    EXPECT_TRUE(successed);
    EXPECT_EQ(pass, EGameBoardUpdatePass::Match);

    successed = gameBoardFSM.querySubPass(pass);
    EXPECT_FALSE(successed);
}

TEST_F(GameBoardFSMTests, CheckRespawnAfterMatch) {
    GameBoardFSM gameBoardFSM;
    auto& state = gameBoardFSM.getState();

    state.isMatchRequested = true;
    state.isRespawnRequested = true;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Match);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Respawn);
    }

    state.isRespawnRequested = false;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_FALSE(successed);
    }
}