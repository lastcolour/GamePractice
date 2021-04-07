#include "GameBoardFSMTests.hpp"
#include "Game/Logics/GameBoardFSM.hpp"

TEST_F(GameBoardFSMTests, CheckMovingState) {
    GameBoardFSM gameBoardFSM;

    auto& state = gameBoardFSM.getState();
    state.hasMovingElems = true;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Move);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_FALSE(successed);
    }
}

TEST_F(GameBoardFSMTests, CheckMatchAfterMove) {
    GameBoardFSM gameBoardFSM;

    auto& state = gameBoardFSM.getState();
    state.hasMovingElems = true;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Move);
    }

    state.hasMovingElems = false;
    state.isMatchRequested = true;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Match);
    }

    state.isMatchRequested = false;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_FALSE(successed);
    }
}

TEST_F(GameBoardFSMTests, CheckMatchAfterMerge) {
    GameBoardFSM gameBoardFSM;

    auto& state = gameBoardFSM.getState();
    state.hasMergingElems = true;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Merge);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_FALSE(successed);
    }

    state.isMatchRequested = true;
    state.hasMergingElems = false;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Match);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_FALSE(successed);
    }
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

TEST_F(GameBoardFSMTests, CheckReturnToMergeAfterRespawn) {
    GameBoardFSM gameBoardFSM;
    auto& state = gameBoardFSM.getState();

    state.hasMergingElems = true;
    state.isRespawnRequested = true;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Merge);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Respawn);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_FALSE(successed);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Merge);
    }
}

TEST_F(GameBoardFSMTests, CheckEnterStaticAfterMove) {
    GameBoardFSM gameBoardFSM;
    auto& state = gameBoardFSM.getState();

    state.hasMovingElems = true;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Move);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_FALSE(successed);
    }

    state.hasMovingElems = false;

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_TRUE(successed);
        EXPECT_EQ(pass, EGameBoardUpdatePass::Static);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.querySubPass(pass);

        EXPECT_FALSE(successed);
    }

    {
        EGameBoardUpdatePass pass = EGameBoardUpdatePass::Static;
        auto successed = gameBoardFSM.queryPass(pass);

        EXPECT_FALSE(successed);
    }
}