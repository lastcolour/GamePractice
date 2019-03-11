#include "GameBoardTests.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Core/JSONNode.hpp"

namespace {
    const char* TEST_CELL_OBJECT = "Simple";
}

struct TestBoardParams {
    Vec2i boardSize;
    float space { 1.f };
    float cellScale { 1.f };
    float moveSpeed { 1.f };
};

class TestGameBoardLogic : public GameBoardLogic {
public:
    TestGameBoardLogic() {}
    virtual ~TestGameBoardLogic() {}

    bool init() {
        JSONNode node; 
        return GameBoardLogic::init(node);
    }

    void updateAfterRemoves() {
        GameBoardLogic::updateAfterRemoves();
    }

    void updateBoard() {
        GameBoardLogic::updateBoard();
    }

    std::vector<BoardElement>& getElements() { return elements; }
    bool removeVerticalLine(const Vec2i& boardPt, int lineLen) {
        return GameBoardLogic::removeVerticalLine(boardPt, lineLen);
    }
    bool removeHorizontalLine(const Vec2i& boardPt, int lineLen) {
        return GameBoardLogic::removeHorizontalLine(boardPt, lineLen);
    }

    void setParams(const TestBoardParams& params) {
        space = params.space;
        boardSize = params.boardSize;
        cellScale = params.cellScale;
        moveSpeed = params.moveSpeed;
        cellObject = TEST_CELL_OBJECT;
    }

    BoardElement* getElem(const Vec2i& boardPt) {
        return GameBoardLogic::getElem(boardPt);
    }

protected:

    bool serialize(const JSONNode& node) override { return true; }
    BoardElemType getElemType() const override { return BoardElemType::Blue; }
};

void GameBoardTests::SetUp() {
    std::unique_ptr<TestGameBoardLogic> boardPtr(new TestGameBoardLogic);
    board = boardPtr.get();
    object.reset(new GameObject("testObj", GetEnv()->getETSystem()->createNewEntityId()));
    board->setGameObject(object.get());
    object->addLogic(std::move(boardPtr));
}

void GameBoardTests::TearDown() {
    object.reset();
    board = nullptr;
}

TEST_F(GameBoardTests, CheckInit) {
    TestBoardParams params;
    params.boardSize = Vec2i(1);
    board->setParams(params);
    ASSERT_TRUE(board->init());

    const auto& elems = board->getElements();
    ASSERT_EQ(elems.size(), 1);
    ASSERT_EQ(elems[0].state, BoardElemState::Static);
    ASSERT_EQ(elems[0].boardPt, Vec2i(0));
}

TEST_F(GameBoardTests, CheckRemoveHorizontalLine) {
    TestBoardParams params;
    params.boardSize = Vec2i(3, 1);
    board->setParams(params);
    ASSERT_TRUE(board->init());

    const auto& elems = board->getElements();
    ASSERT_EQ(elems.size(), 3);

    ASSERT_TRUE(board->removeHorizontalLine(Vec2i(0, 0), 3));

    board->updateAfterRemoves();

    ASSERT_EQ(elems.size(), 3);

    for(auto& elem : elems) {
        ASSERT_EQ(elem.state, BoardElemState::Moving);
        ASSERT_EQ(elem.boardPt, Vec2i(elem.movePt.x, elem.movePt.y + 1));
    }
}

TEST_F(GameBoardTests, CheckRemoveVerticalLine) {
    TestBoardParams params;
    params.boardSize = Vec2i(1, 3);
    board->setParams(params);
    ASSERT_TRUE(board->init());

    const auto& elems = board->getElements();
    ASSERT_EQ(elems.size(), 3);

    ASSERT_TRUE(board->removeVerticalLine(Vec2i(0, 0), 3));

    board->updateAfterRemoves();

    ASSERT_EQ(elems.size(), 3);

    for(auto& elem : elems) {
        ASSERT_EQ(elem.state, BoardElemState::Moving);
        if(elem.movePt == Vec2i(0)) {
            ASSERT_EQ(elem.boardPt, Vec2i(0, 3));
        } else if(elem.movePt == Vec2i(0, 1)) {
            ASSERT_EQ(elem.boardPt, Vec2i(0, 4));
        } else if(elem.movePt == Vec2i(0, 2)) {
            ASSERT_EQ(elem.boardPt, Vec2i(0, 5));
        } else {
            FAIL();
        }
    }
}

TEST_F(GameBoardTests, CheckRetargetAfterRemove) {
    TestBoardParams params;
    params.boardSize = Vec2i(1, 3);
    board->setParams(params);
    ASSERT_TRUE(board->init());

    auto elem1 = board->getElem(Vec2i(0, 0));
    ASSERT_TRUE(elem1);
    elem1->state = BoardElemState::Void;

    board->updateAfterRemoves();

    auto elem2 = board->getElem(Vec2i(0, 1));
    ASSERT_TRUE(elem2);
    ASSERT_EQ(elem2->state, BoardElemState::Moving);
    ASSERT_EQ(elem2->movePt, Vec2i(0, 0));

    auto elem3 = board->getElem(Vec2i(0, 2));
    ASSERT_TRUE(elem3);
    ASSERT_EQ(elem3->state, BoardElemState::Moving);
    ASSERT_EQ(elem3->movePt, Vec2i(0, 1));

    elem2->state = BoardElemState::Void;

    board->updateAfterRemoves();

    ASSERT_TRUE(elem3);
    ASSERT_EQ(elem3->state, BoardElemState::Moving);
    ASSERT_EQ(elem3->movePt, Vec2i(0, 0));
}

TEST_F(GameBoardTests, CheckMoving) {
    TestBoardParams params;
    params.boardSize = Vec2i(1, 3);
    params.moveSpeed = 1.f;
    board->setParams(params);
    ASSERT_TRUE(board->init());
    ASSERT_TRUE(board->removeVerticalLine(Vec2i(0, 0), 3));
    board->updateAfterRemoves();

    ASSERT_TRUE(board->getElem(Vec2i(0, 5)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 4)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 3)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 2)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 1)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 0)));

    board->ET_onGameTick(0.49f);

    ASSERT_TRUE(board->getElem(Vec2i(0, 5)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 4)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 3)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 2)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 1)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 0)));

    board->ET_onGameTick(0.52f);

    ASSERT_FALSE(board->getElem(Vec2i(0, 5)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 4)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 3)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 2)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 1)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 0)));

    board->ET_onGameTick(1.f);

    ASSERT_FALSE(board->getElem(Vec2i(0, 5)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 4)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 3)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 2)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 1)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 0)));

    board->ET_onGameTick(1.f);

    ASSERT_FALSE(board->getElem(Vec2i(0, 5)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 4)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 3)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 2)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 1)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 0)));
}