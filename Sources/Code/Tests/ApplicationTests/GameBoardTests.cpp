#include "GameBoardTests.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Core/JSONNode.hpp"
#include "Entity.hpp"
#include "Logics/UIBox.hpp"
#include "Game/ETGameElem.hpp"
#include "Game/Logics/GameBoardElemsPool.hpp"

namespace {

const char* TEST_OBJECT_1 = "Entities/Game/Blocks/Blue.json";

EBoardElemState getElemState(const BoardElement& elem) {
    EBoardElemState state = EBoardElemState::Static;
    ET_SendEventReturn(state, elem.entId, &ETGameBoardElem::ET_getElemState);
    return state;
}

void setElemState(const BoardElement& elem, EBoardElemState state) {
    ET_SendEvent(elem.entId, &ETGameBoardElem::ET_setElemState, state);
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
        isBoardStatic = false;
        ET_onGameTick(0.f);
    }

    void finishElemsFailling() {
        ET_onGameTick(10.f);
        ET_SendEvent(&ETGameTimerEvents::ET_onGameTick, 10.f);
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
            setElemState(*elem, EBoardElemState::Destroyed);
            ++elemPt.y;
        }
        updateAfterRemoves();
    }

    void removeHorizontalLine(const Vec2i& boardPt, int lineLen) {
        Vec2i elemPt = boardPt;
        for(int i = 0; i < lineLen; ++i) {
            auto elem = getElem(elemPt);
            assert(elem && "Try remove invalid elem");
            setElemState(*elem, EBoardElemState::Destroyed);
            ++elemPt.x;
        }
        updateAfterRemoves();
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
    ASSERT_EQ(getElemState(elems[0]), EBoardElemState::Static);
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

    ASSERT_EQ(elems.size(), 3u);

    for(auto& elem : elems) {
        EXPECT_EQ(getElemState(elem), EBoardElemState::Falling);
    }

    board->finishElemsFailling();

    for(auto& elem : elems) {
        EXPECT_EQ(getElemState(elem), EBoardElemState::Landing);
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

    ASSERT_EQ(elems.size(), 3u);

    for(auto& elem : elems) {
        EXPECT_EQ(getElemState(elem), EBoardElemState::Falling);
    }

    board->finishElemsFailling();

    for(auto& elem : elems) {
        EXPECT_EQ(getElemState(elem), EBoardElemState::Landing);
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

    setElemState(*elem2, EBoardElemState::Destroyed);

    board->updateAfterRemoves();

    {
        ASSERT_EQ(getElemState(*elem3), EBoardElemState::Falling);
    }
    {
        ASSERT_EQ(getElemState(*elem2), EBoardElemState::Falling);
    }
    {
        ASSERT_EQ(getElemState(*elem1), EBoardElemState::Static);
        ASSERT_EQ(elem1->boardPt, Vec2i(0, 0));
    }

    setElemState(*elem1, EBoardElemState::Destroyed);

    board->updateAfterRemoves();

    {
        ASSERT_EQ(getElemState(*elem3), EBoardElemState::Falling);
    }
    {
        ASSERT_EQ(getElemState(*elem2), EBoardElemState::Falling);
    }
    {
        ASSERT_EQ(getElemState(*elem1), EBoardElemState::Falling);
    }

    board->finishElemsFailling();

    {
        ASSERT_EQ(getElemState(*elem3), EBoardElemState::Landing);
        ASSERT_EQ(elem3->boardPt, Vec2i(0, 2));
    }
    {
        ASSERT_EQ(getElemState(*elem2), EBoardElemState::Landing);
        ASSERT_EQ(elem2->boardPt, Vec2i(0, 1));
    }
    {
        ASSERT_EQ(getElemState(*elem1), EBoardElemState::Landing);
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

    setElemState(*elem2, EBoardElemState::Destroyed);
    board->updateAfterRemoves();

    board->ET_onGameTick(0.5f);

    setElemState(*elem1, EBoardElemState::Destroyed);
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
    board->ET_resize(uiBox);

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