#include "BronKerboschTests.hpp"
#include "Game/Logics/BronKerbosch.hpp"

TEST_F(BronKerboschTests, TestAdjacencyMatrix) {
    AdjacencyMatrix adjMat(4);

    ASSERT_EQ(adjMat.vertexCount(), 4);

    adjMat.addEdge(0, 1);
    adjMat.addEdge(2, 3);

    EXPECT_FALSE(adjMat.hasEdge(0, 0));
    EXPECT_FALSE(adjMat.hasEdge(1, 1));
    EXPECT_FALSE(adjMat.hasEdge(2, 2));
    EXPECT_FALSE(adjMat.hasEdge(3, 3));

    EXPECT_TRUE(adjMat.hasEdge(0, 1));
    EXPECT_TRUE(adjMat.hasEdge(1, 0));

    EXPECT_TRUE(adjMat.hasEdge(2, 3));
    EXPECT_TRUE(adjMat.hasEdge(3, 2));

    EXPECT_FALSE(adjMat.hasEdge(0, 2));
    EXPECT_FALSE(adjMat.hasEdge(2, 0));
    EXPECT_FALSE(adjMat.hasEdge(1, 3));
    EXPECT_FALSE(adjMat.hasEdge(3, 0));

    EXPECT_EQ(adjMat.edgeCount(), 2);
}

TEST_F(BronKerboschTests, CheckTwoVertexGraph) {
    BronKerboschRequest req;
    req.adjMat.reset(2);

    BronKerbosch(req);

    auto& maxClique = req.result;

    ASSERT_EQ(maxClique.size(), 1u);
}

TEST_F(BronKerboschTests, CheckOneClique) {
    BronKerboschRequest req;

    req.adjMat.reset(3);
    req.adjMat.addEdge(0, 1);
    req.adjMat.addEdge(0, 2);
    req.adjMat.addEdge(1, 2);

    BronKerbosch(req);

    auto& maxClique = req.result;
    ASSERT_EQ(maxClique.size(), 3u);

    {
        std::set<int> s(maxClique.begin(), maxClique.end());

        EXPECT_TRUE(s.count(0));
        EXPECT_TRUE(s.count(1));
        EXPECT_TRUE(s.count(2));
    }
}

TEST_F(BronKerboschTests, CheckTwoClique) {
    BronKerboschRequest req;

    req.adjMat.reset(7);
    req.adjMat.addEdge(0, 1);
    req.adjMat.addEdge(0, 2);
    req.adjMat.addEdge(1, 2);
    req.adjMat.addEdge(2, 3);

    req.adjMat.addEdge(3, 5);
    req.adjMat.addEdge(3, 4);
    req.adjMat.addEdge(3, 6);

    req.adjMat.addEdge(6, 5);
    req.adjMat.addEdge(6, 4);

    req.adjMat.addEdge(5, 4);

    BronKerbosch(req);

    auto& maxClique = req.result;

    {
        std::set<int> s(maxClique.begin(), maxClique.end());

        EXPECT_TRUE(s.count(3));
        EXPECT_TRUE(s.count(4));
        EXPECT_TRUE(s.count(5));
        EXPECT_TRUE(s.count(6));
    }
}