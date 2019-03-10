#include "GameBoardTests.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Core/JSONNode.hpp"

namespace {
    const char* TEST_CELL_OBJECT = "Simple";
}

struct TestBoardParams {
    Vec2i boardSize;
    float space;
    float cellScale;
};

class TestGameBoardLogic : public GameBoardLogic {
public:
    TestGameBoardLogic() {}
    virtual ~TestGameBoardLogic() {}

    bool init() {
        JSONNode node; 
        return GameBoardLogic::init(node);
    }

    std::vector<BoardElement>& getElements() { return elements; }
    void removeVerticalLines(const Vec2i& boardPt, int lineLen) {
        GameBoardLogic::removeVerticalLines(boardPt, lineLen);
    }
    void removeHorizontalLines(const Vec2i& boardPt, int lineLen) {
        GameBoardLogic::removeHorizontalLines(boardPt, lineLen);
    }

    size_t getElemSize() const { return elements.size(); }

    void setParams(const TestBoardParams& params) {
        space = params.space;
        boardSize = params.boardSize;
        cellScale = params.cellScale;
        cellObject = TEST_CELL_OBJECT;
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
    params.cellScale = 1.f;
    params.space = 1.f;
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
    params.cellScale = 1.f;
    params.space = 1.f;
    board->setParams(params);
    ASSERT_TRUE(board->init());

    board->removeHorizontalLines(Vec2i(0, 0), 3);
    const auto& elems = board->getElements();
    for(auto& elem : elems) {
        ASSERT_TRUE(elem.state == BoardElemState::Void);
    }
}

TEST_F(GameBoardTests, CheckRemoveVerticalLine) {
    TestBoardParams params;
    params.boardSize = Vec2i(1, 3);
    params.cellScale = 1.f;
    params.space = 1.f;
    board->setParams(params);
    ASSERT_TRUE(board->init());

    board->removeVerticalLines(Vec2i(0, 0), 3);
    const auto& elems = board->getElements();
    for(auto& elem : elems) {
        ASSERT_TRUE(elem.state == BoardElemState::Void);
    }
}