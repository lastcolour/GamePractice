#include "GameBoardMatcherTests.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/Logics/GameBoardUtils.hpp"
#include "Game/Logics/GameBoardMatchLogic.hpp"
#include "Entity/ETEntityManager.hpp"

namespace {

const char* TEST_ELEM = "Entities/Game/Blocks/Blue.json";

class TestGameBoard : public GameBoardLogic {
public:

    TestGameBoard() {}
    virtual ~TestGameBoard() {}

    void init() override {
        return;
    }

    void createBoard(const std::vector<std::string>& setup) {
        createSetup = setup;
        boardSize.y = static_cast<int>(setup.size());
        boardSize.x = static_cast<int>(setup[0].size());
        GameBoardLogic::init();
    }

    std::vector<std::string> getSetup() const {
        std::vector<std::string> res;
        for(int i = boardSize.y - 1; i >= 0; --i) {
            std::string rowStr;
            for(int j = 0; j < boardSize.x; ++j) {
                auto elem = getElem(Vec2i(j, i));
                if(!elem) {
                    rowStr += 'E';
                } else {
                    auto state = GameUtils::GetElemState(elem->entId);
                    if(state == EBoardElemState::Static) {
                        rowStr += 'S';
                    } else if(state == EBoardElemState::Destroying) {
                        rowStr += 'D';
                    } else {
                        rowStr += '0';
                    }
                }
            }
            res.emplace_back(std::move(rowStr));
        }
        return res;
    }

protected:

    BoardElement createNewElement(const Vec2i& boardPt) const override {
        BoardElement elem;
        ET_SendEventReturn(elem.entId, &ETEntityManager::ET_createEntity, TEST_ELEM);
        if(!elem.entId.isValid()) {
            return elem;
        }
        setElemBoardPos(elem, boardPt);

        ET_SendEvent(elem.entId, &ETEntity::ET_setParent, getEntityId());

        auto ch = createSetup[createSetup.size() - boardPt.y - 1][boardPt.x];
    
        if(ch == 'D') {
            GameUtils::SetElemState(elem.entId, EBoardElemState::Destroying);
        } else if(ch == 'S') {
            GameUtils::SetElemState(elem.entId, EBoardElemState::Static);
        } else {
            return BoardElement();
        }
        return elem;
    }

private:

    std::vector<std::string> createSetup;
};

} // namespace

TEST_F(GameBoardMatcherTests, TestOnly3HLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "SSS",
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDD");
}

TEST_F(GameBoardMatcherTests, TestOnly3VLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "S",
        "S",
        "S"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "D");
    EXPECT_EQ(setup[1], "D");
    EXPECT_EQ(setup[2], "D");
}

TEST_F(GameBoardMatcherTests, Test3VLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDD",
        "DSD",
        "DSD",
        "DSD",
        "DDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDD");
    EXPECT_EQ(setup[1], "DDD");
    EXPECT_EQ(setup[2], "DDD");
    EXPECT_EQ(setup[3], "DDD");
    EXPECT_EQ(setup[4], "DDD");
}

TEST_F(GameBoardMatcherTests, Test4HLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDDDDD",
        "DSSSSD",
        "DDDDDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDDDDD");
    EXPECT_EQ(setup[1], "DDDDDD");
    EXPECT_EQ(setup[2], "DDDDDD");
}

TEST_F(GameBoardMatcherTests, Test4VLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDD",
        "DSD",
        "DSD",
        "DSD",
        "DSD",
        "DDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDD");
    EXPECT_EQ(setup[1], "DDD");
    EXPECT_EQ(setup[2], "DDD");
    EXPECT_EQ(setup[3], "DDD");
    EXPECT_EQ(setup[4], "DDD");
    EXPECT_EQ(setup[5], "DDD");
}

TEST_F(GameBoardMatcherTests, Test5HLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDDDDDD",
        "DSSSSSD",
        "DDDDDDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDDDDDD");
    EXPECT_EQ(setup[1], "DDDDDDD");
    EXPECT_EQ(setup[2], "DDDDDDD");
}

TEST_F(GameBoardMatcherTests, Test5VLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDD",
        "DSD",
        "DSD",
        "DSD",
        "DSD",
        "DSD",
        "DDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDD");
    EXPECT_EQ(setup[1], "DDD");
    EXPECT_EQ(setup[2], "DDD");
    EXPECT_EQ(setup[3], "DDD");
    EXPECT_EQ(setup[4], "DDD");
    EXPECT_EQ(setup[5], "DDD");
    EXPECT_EQ(setup[6], "DDD");
}

TEST_F(GameBoardMatcherTests, Test3MidCrossLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDDDD",
        "DDSDD",
        "DSSSD",
        "DDSDD",
        "DDDDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDDDD");
    EXPECT_EQ(setup[1], "DDDDD");
    EXPECT_EQ(setup[2], "DDDDD");
    EXPECT_EQ(setup[3], "DDDDD");
    EXPECT_EQ(setup[4], "DDDDD");
}

TEST_F(GameBoardMatcherTests, Test3MidTDownLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDDDD",
        "DSSSD",
        "DDSDD",
        "DDSDD",
        "DDDDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDDDD");
    EXPECT_EQ(setup[1], "DDDDD");
    EXPECT_EQ(setup[2], "DDDDD");
    EXPECT_EQ(setup[3], "DDDDD");
    EXPECT_EQ(setup[4], "DDDDD");
}

TEST_F(GameBoardMatcherTests, Test3MidTUpLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDDDD",
        "DDSDD",
        "DDSDD",
        "DSSSD",
        "DDDDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDDDD");
    EXPECT_EQ(setup[1], "DDDDD");
    EXPECT_EQ(setup[2], "DDDDD");
    EXPECT_EQ(setup[3], "DDDDD");
    EXPECT_EQ(setup[4], "DDDDD");
}

TEST_F(GameBoardMatcherTests, Test3LeftLUpLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDDDD",
        "DSDDD",
        "DSDDD",
        "DSSSD",
        "DDDDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDDDD");
    EXPECT_EQ(setup[1], "DDDDD");
    EXPECT_EQ(setup[2], "DDDDD");
    EXPECT_EQ(setup[3], "DDDDD");
    EXPECT_EQ(setup[4], "DDDDD");
}

TEST_F(GameBoardMatcherTests, Test3RightLUpLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDDDD",
        "DDDSD",
        "DDDSD",
        "DSSSD",
        "DDDDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDDDD");
    EXPECT_EQ(setup[1], "DDDDD");
    EXPECT_EQ(setup[2], "DDDDD");
    EXPECT_EQ(setup[3], "DDDDD");
    EXPECT_EQ(setup[4], "DDDDD");
}

TEST_F(GameBoardMatcherTests, Test3LeftLDownLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDDDD",
        "DSSSD",
        "DSDDD",
        "DSDDD",
        "DDDDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDDDD");
    EXPECT_EQ(setup[1], "DDDDD");
    EXPECT_EQ(setup[2], "DDDDD");
    EXPECT_EQ(setup[3], "DDDDD");
    EXPECT_EQ(setup[4], "DDDDD");
}

TEST_F(GameBoardMatcherTests, Test3RightLDownLine) {
    auto entity = createVoidObject();
    auto testGameBoard = entity->addCustomLogic<TestGameBoard>();
    auto matcher = entity->addCustomLogic<GameBoardMatchLogic>();

    testGameBoard->createBoard({
        "DDDDD",
        "DSSSD",
        "DDDSD",
        "DDDSD",
        "DDDDD"
    });

    matcher->ET_destoryMatchedElems();

    auto setup = testGameBoard->getSetup();
    EXPECT_EQ(setup[0], "DDDDD");
    EXPECT_EQ(setup[1], "DDDDD");
    EXPECT_EQ(setup[2], "DDDDD");
    EXPECT_EQ(setup[3], "DDDDD");
    EXPECT_EQ(setup[4], "DDDDD");
}