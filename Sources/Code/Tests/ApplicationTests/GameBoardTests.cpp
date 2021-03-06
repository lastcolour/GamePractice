#include "GameBoardTests.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Core/JSONNode.hpp"
#include "Entity.hpp"
#include "Logics/UIBox.hpp"
#include "Game/ETGameElem.hpp"
#include "Game/Logics/GameBoardElemsPool.hpp"

#include <algorithm>

namespace {

const char* TEST_OBJECT_1 = "Game/Blocks/Blue.json";

EBoardElemMoveState getElemMoveState(const BoardElement& elem) {
    EBoardElemMoveState state = EBoardElemMoveState::Static;
    ET_SendEventReturn(state, elem.entId, &ETGameBoardElem::ET_getMoveState);
    return state;
}

void setElemLifeState(const BoardElement& elem, EBoardElemLifeState state) {
    ET_SendEvent(elem.entId, &ETGameBoardElem::ET_setLifeState, state);
}

} // namespace

struct TestBoardParams {
    Vec2i boardSize;
    float cellScale { 1.f };
    float moveSpeed { 1.f };
};

class TestGameBoardLogic : public GameBoardLogic {
public:
    virtual ~TestGameBoardLogic() {
    }

    void updateAfterRemoves() {
        GameBoardLogic::updateAfterRemoves();
    }

    std::vector<BoardElement> getElements() {
        std::vector<BoardElement> res;
        for(auto& col : columns) {
            for(auto& elem : col) {
                res.push_back(elem);
            }
        }
        return res;
    }

    Vec2i getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const {
        return GameBoardLogic::getBoardPosFromPos(boardPt, pt);
    }

    Vec3 getPosFromBoardPos(const Vec2i& pt) const {
        return GameBoardLogic::ET_getPosFromBoardPos(pt);
    }

    void removeVerticalLine(const Vec2i& boardPt, int lineLen) {
        Vec2i elemPt = boardPt;
        for(int i = 0; i < lineLen; ++i) {
            auto elem = getElem(elemPt);
            assert(elem && "Try remove invalid elem");
            setElemLifeState(*elem, EBoardElemLifeState::Void);
            ++elemPt.y;
        }
    }

    void removeHorizontalLine(const Vec2i& boardPt, int lineLen) {
        Vec2i elemPt = boardPt;
        for(int i = 0; i < lineLen; ++i) {
            auto elem = getElem(elemPt);
            assert(elem && "Try remove invalid elem");
            setElemLifeState(*elem, EBoardElemLifeState::Void);
            ++elemPt.x;
        }
    }

    int getCellSize() const {
        return cellSize;
    }

    void setParams(const TestBoardParams& params) {
        boardSize = params.boardSize;
        cellScale = params.cellScale;
        moveSpeed = params.moveSpeed;
    }

    BoardElement* getElem(const Vec2i& boardPt) {
        return GameBoardLogic::getElem(boardPt);
    }
};

void GameBoardTests::SetUp() {
    entity = createVoidObject();
    {
        auto pool = entity->addCustomLogic<GameBoardElemsPool>();
        ASSERT_TRUE(pool);

        ElementDescriptor elemDescr;
        elemDescr.weight = 1;
        elemDescr.object = TEST_OBJECT_1;
        pool->ET_addElemsToSpawn(elemDescr);
    }
    {
        board = entity->addCustomLogic<TestGameBoardLogic>();
        ASSERT_TRUE(board);
    }
}

TEST_F(GameBoardTests, CheckInit) {
    TestBoardParams params;
    params.boardSize = Vec2i(1);
    board->setParams(params);
    board->init();

    const auto& elems = board->getElements();
    ASSERT_EQ(elems.size(), 1u);
    ASSERT_EQ(getElemMoveState(elems[0]), EBoardElemMoveState::Static);
    ASSERT_EQ(elems[0].boardPt, Vec2i(0));
}

TEST_F(GameBoardTests, CheckPosConverts) {
    TestBoardParams params;
    params.boardSize = Vec2i(1);
    board->setParams(params);
    board->init();

    auto pt = board->getPosFromBoardPos(Vec2i(0));
    auto boardPt = board->getBoardPosFromPos(Vec2i(0), pt);
    ASSERT_EQ(boardPt, Vec2i(0));
}

TEST_F(GameBoardTests, CheckRemoveHorizontalLine) {
    TestBoardParams params;
    params.boardSize = Vec2i(3, 1);
    board->setParams(params);
    board->init();

    const auto& elems = board->getElements();
    ASSERT_EQ(elems.size(), 3u);

    board->removeHorizontalLine(Vec2i(0, 0), 3);
    board->updateAfterRemoves();

    ASSERT_EQ(elems.size(), 3u);

    for(auto& elem : elems) {
        EXPECT_EQ(getElemMoveState(elem), EBoardElemMoveState::Falling);
    }

    board->ET_onGameTick(10.f);

    for(auto& elem : elems) {
        EXPECT_EQ(getElemMoveState(elem), EBoardElemMoveState::Static);
    }

    EXPECT_TRUE(board->getElem(Vec2i(0, 0)));
    EXPECT_TRUE(board->getElem(Vec2i(1, 0)));
    EXPECT_TRUE(board->getElem(Vec2i(2, 0)));
}

TEST_F(GameBoardTests, CheckRemoveVerticalLine) {
    TestBoardParams params;
    params.boardSize = Vec2i(1, 3);
    board->setParams(params);
    board->init();

    const auto& elems = board->getElements();
    ASSERT_EQ(elems.size(), 3u);

    board->removeVerticalLine(Vec2i(0, 0), 3);

    board->updateAfterRemoves();

    ASSERT_EQ(elems.size(), 3u);

    for(auto& elem : elems) {
        EXPECT_EQ(getElemMoveState(elem), EBoardElemMoveState::Falling);
    }

    board->ET_onGameTick(10.f);

    for(auto& elem : elems) {
        EXPECT_EQ(getElemMoveState(elem), EBoardElemMoveState::Static);
    }

    EXPECT_TRUE(board->getElem(Vec2i(0, 0)));
    EXPECT_TRUE(board->getElem(Vec2i(0, 1)));
    EXPECT_TRUE(board->getElem(Vec2i(0, 2)));
}

TEST_F(GameBoardTests, CheckRetargetAfterRemove) {
    TestBoardParams params;
    params.boardSize = Vec2i(1, 3);
    board->setParams(params);
    board->init();

    auto elem1 = board->getElem(Vec2i(0, 0));
    ASSERT_TRUE(elem1);

    auto elem2 = board->getElem(Vec2i(0, 1));
    ASSERT_TRUE(elem2);

    auto elem3 = board->getElem(Vec2i(0, 2));
    ASSERT_TRUE(elem3);

    setElemLifeState(*elem2, EBoardElemLifeState::Void);
    board->updateAfterRemoves();
    board->ET_onGameTick(0.1f);

    {
        ASSERT_EQ(getElemMoveState(*elem3), EBoardElemMoveState::Falling);
    }
    {
        ASSERT_EQ(getElemMoveState(*elem2), EBoardElemMoveState::Falling);
    }
    {
        ASSERT_EQ(getElemMoveState(*elem1), EBoardElemMoveState::Static);
        ASSERT_EQ(elem1->boardPt, Vec2i(0, 0));
    }

    setElemLifeState(*elem1, EBoardElemLifeState::Void);
    board->updateAfterRemoves();
    board->ET_onGameTick(0.1f);

    {
        ASSERT_EQ(getElemMoveState(*elem3), EBoardElemMoveState::Falling);
    }
    {
        ASSERT_EQ(getElemMoveState(*elem2), EBoardElemMoveState::Falling);
    }
    {
        ASSERT_EQ(getElemMoveState(*elem1), EBoardElemMoveState::Falling);
    }

    board->ET_onGameTick(10.f);

    {
        ASSERT_EQ(getElemMoveState(*elem3), EBoardElemMoveState::Static);
        ASSERT_EQ(elem3->boardPt, Vec2i(0, 2));
    }
    {
        ASSERT_EQ(getElemMoveState(*elem2), EBoardElemMoveState::Static);
        ASSERT_EQ(elem2->boardPt, Vec2i(0, 1));
    }
    {
        ASSERT_EQ(getElemMoveState(*elem1), EBoardElemMoveState::Static);
        ASSERT_EQ(elem1->boardPt, Vec2i(0, 0));
    }
}

TEST_F(GameBoardTests, CheckSpawnNewWhenMoving) {
    TestBoardParams params;
    params.boardSize = Vec2i(1, 3);
    params.moveSpeed = 1.f;
    board->setParams(params);
    board->init();

    auto elem1 = board->getElem(Vec2i(0, 0));
    ASSERT_TRUE(elem1);

    auto elem2 = board->getElem(Vec2i(0, 1));
    ASSERT_TRUE(elem2);

    auto elem3 = board->getElem(Vec2i(0, 2));
    ASSERT_TRUE(elem3);

    setElemLifeState(*elem2, EBoardElemLifeState::Void);
    board->updateAfterRemoves();

    board->ET_onGameTick(0.5f);

    setElemLifeState(*elem1, EBoardElemLifeState::Void);
    board->updateAfterRemoves();

    Transform tm2;
    ET_SendEventReturn(tm2, elem2->entId, &ETEntity::ET_getTransform);

    Transform tm1;
    ET_SendEventReturn(tm1, elem1->entId, &ETEntity::ET_getTransform);

    Vec3 ptDiff = tm1.pt - tm2.pt;
    ASSERT_FLOAT_EQ(ptDiff.x, 0.f);
    ASSERT_FLOAT_EQ(std::abs(ptDiff.y), static_cast<float>(board->getCellSize()));
    ASSERT_FLOAT_EQ(ptDiff.z, 0.f);
}

TEST_F(GameBoardTests, CheckRelativeLocationToUIBox) {
    auto uiBoxPtr = entity->addCustomLogic<UIBox>();
    ASSERT_TRUE(uiBoxPtr);

    UIBoxStyle style;
    style.width = 0.5f;
    style.widthInv = UIBoxSizeInvariant::Relative;
    style.height = 0.5f;
    style.heightInv = UIBoxSizeInvariant::Relative;
    uiBoxPtr->ET_setStyle(style);

    TestBoardParams params;
    params.boardSize = Vec2i(1, 1);
    params.cellScale = 1.f;
    board->setParams(params);
    board->init();

    const auto& uiBox = uiBoxPtr->ET_getBox();
    board->ET_onBoxChanged(uiBox);

    auto cellSize = board->getCellSize();

    auto uiBoxSize = uiBox.getSize();
    auto expectedCellSize = std::min(uiBoxSize.x, uiBoxSize.y);
    EXPECT_EQ(cellSize, expectedCellSize);

    auto elem = board->getElem(Vec2i(0, 0));
    Transform tm;
    ET_SendEventReturn(tm, elem->entId, &ETEntity::ET_getTransform);

    auto uiBoxCenter = Vec3(static_cast<float>(uiBox.getCenter().x),
        static_cast<float>(uiBox.getCenter().y), 0.f);
    ASSERT_EQ(tm.pt, uiBoxCenter);
}