#include "GameBoardTests.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Core/JSONNode.hpp"
#include "Game/GameObject.hpp"
#include "UI/Logics/UIBox.hpp"

namespace {
    const char* TEST_CELL_OBJECT = "Game/Simple.json";
}

struct TestBoardParams {
    Vec2i boardSize;
    float cellScale { 1.f };
    float moveSpeed { 1.f };
};

class TestGameBoardLogic : public GameBoardLogic {
public:
    virtual ~TestGameBoardLogic() = default;

    void updateAfterRemoves() {
        GameBoardLogic::updateAfterRemoves();
    }

    void updateBoard() {
        GameBoardLogic::updateBoard();
    }

    std::vector<BoardElement>& getElements() {
        return elements;
    }
    Vec2i getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const {
        return GameBoardLogic::getBoardPosFromPos(boardPt, pt);
    }
    Vec3 getPosFromBoardPos(const Vec2i& pt) const {
        return GameBoardLogic::getPosFromBoardPos(pt);
    }
    bool removeVerticalLine(const Vec2i& boardPt, int lineLen) {
        return GameBoardLogic::removeVerticalLine(boardPt, lineLen);
    }
    bool removeHorizontalLine(const Vec2i& boardPt, int lineLen) {
        return GameBoardLogic::removeHorizontalLine(boardPt, lineLen);
    }
    int getCellSize() const {
        return cellSize;
    }

    void setParams(const TestBoardParams& params) {
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
    object = createVoidObject();
    std::unique_ptr<TestGameBoardLogic> boardPtr(new TestGameBoardLogic);
    board = boardPtr.get();
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
    ASSERT_EQ(elems.size(), 1u);
    ASSERT_EQ(elems[0].state, BoardElemState::Static);
    ASSERT_EQ(elems[0].boardPt, Vec2i(0));
}

TEST_F(GameBoardTests, CheckPosConverts) {
    TestBoardParams params;
    params.boardSize = Vec2i(1);
    board->setParams(params);
    ASSERT_TRUE(board->init());

    auto pt = board->getPosFromBoardPos(Vec2i(0));
    auto boardPt = board->getBoardPosFromPos(Vec2i(0), pt);
    ASSERT_EQ(boardPt, Vec2i(0));
}

TEST_F(GameBoardTests, CheckRemoveHorizontalLine) {
    TestBoardParams params;
    params.boardSize = Vec2i(3, 1);
    board->setParams(params);
    ASSERT_TRUE(board->init());

    const auto& elems = board->getElements();
    ASSERT_EQ(elems.size(), 3u);

    ASSERT_TRUE(board->removeHorizontalLine(Vec2i(0, 0), 3));

    board->updateAfterRemoves();

    ASSERT_EQ(elems.size(), 3u);

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
    ASSERT_EQ(elems.size(), 3u);

    ASSERT_TRUE(board->removeVerticalLine(Vec2i(0, 0), 3));

    board->updateAfterRemoves();

    ASSERT_EQ(elems.size(), 3u);

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
    params.boardSize = Vec2i(1, 2);
    params.moveSpeed = 1.f;
    board->setParams(params);
    ASSERT_TRUE(board->init());
    ASSERT_TRUE(board->removeVerticalLine(Vec2i(0, 0), 2));
    board->updateAfterRemoves();

    ASSERT_TRUE(board->getElem(Vec2i(0, 3)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 2)));;
    ASSERT_FALSE(board->getElem(Vec2i(0, 1)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 0)));

    board->ET_onTick(0.5f);

    ASSERT_TRUE(board->getElem(Vec2i(0, 3)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 2)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 1)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 0)));

    board->ET_onTick(0.5f);

    ASSERT_FALSE(board->getElem(Vec2i(0, 3)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 2)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 1)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 0)));

    board->ET_onTick(1.f);

    ASSERT_FALSE(board->getElem(Vec2i(0, 3)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 2)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 1)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 0)));
}

TEST_F(GameBoardTests, CheckSpawnNewWhenMoving) {
    TestBoardParams params;
    params.boardSize = Vec2i(1, 3);
    params.moveSpeed = 1.f;
    board->setParams(params);
    ASSERT_TRUE(board->init());

    ASSERT_TRUE(board->removeVerticalLine(Vec2i(0, 1), 1));
    board->updateAfterRemoves();

    ASSERT_TRUE(board->getElem(Vec2i(0, 3)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 2)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 1)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 0)));

    board->ET_onTick(0.5f);

    ASSERT_TRUE(board->removeVerticalLine(Vec2i(0, 0), 1));
    board->updateAfterRemoves();

    ASSERT_TRUE(board->getElem(Vec2i(0, 4)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 3)));
    ASSERT_TRUE(board->getElem(Vec2i(0, 2)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 1)));
    ASSERT_FALSE(board->getElem(Vec2i(0, 0)));

    auto elem1 = board->getElem(Vec2i(0, 3));
    Transform tm1;
    ET_SendEventReturn(tm1, elem1->entId, &ETGameObject::ET_getTransform);

    auto elem2 = board->getElem(Vec2i(0, 4));
    Transform tm2;
    ET_SendEventReturn(tm2, elem2->entId, &ETGameObject::ET_getTransform);

    Vec3 ptDiff = tm2.pt - tm1.pt;
    ASSERT_FLOAT_EQ(ptDiff.x, 0.f);
    ASSERT_FLOAT_EQ(ptDiff.y, static_cast<float>(board->getCellSize()));
    ASSERT_FLOAT_EQ(ptDiff.z, 0.f);
}

TEST_F(GameBoardTests, CheckGameBoardConnections) {
    TestBoardParams params;
    params.boardSize = Vec2i(1);
    params.moveSpeed = 1.f;
    board->setParams(params);
    ASSERT_TRUE(board->init());

    auto entId = board->getEntityId();

    ASSERT_TRUE(ET_IsExistNode<ETInputEvents>(entId));
    ASSERT_TRUE(ET_IsExistNode<ETGameObjectEvents>(entId));
    ASSERT_TRUE(ET_IsExistNode<ETUIBoxEvents>(entId));
    ASSERT_TRUE(ET_IsExistNode<ETTimerEvents>(entId));
}

TEST_F(GameBoardTests, CheckRelativeLocationToUIBox) {
    std::unique_ptr<UIBox> uiBoxLogicPtr(new UIBox);
    auto uiBoxPtr = uiBoxLogicPtr.get();
    object->addLogic(std::move(uiBoxLogicPtr));
    UIStyle style;
    style.size = Vec2(0.5f);
    style.sizeInv = SizeInvariant::RelativeBiggestSquare;
    style.yAlignType = YAlignType::Top;
    uiBoxPtr->ET_setStyle(style);
    ASSERT_TRUE(uiBoxPtr->init());

    TestBoardParams params;
    params.boardSize = Vec2i(1, 1);
    params.cellScale = 1.f;
    board->setParams(params);
    ASSERT_TRUE(board->init());

    const auto& uiBox = uiBoxPtr->ET_getAabb2di();
    auto cellSize = board->getCellSize();

    auto uiBoxSize = uiBox.getSize();
    EXPECT_EQ(cellSize, uiBoxSize.x);
    EXPECT_EQ(cellSize, uiBoxSize.y);

    auto elem = board->getElem(Vec2i(0, 0));
    Transform tm;
    ET_SendEventReturn(tm, elem->entId, &ETGameObject::ET_getTransform);

    auto uiBoxCenter = Vec3(static_cast<float>(uiBox.getCenter().x),
        static_cast<float>(uiBox.getCenter().y), 0.f);
    ASSERT_EQ(tm.pt, uiBoxCenter);
}