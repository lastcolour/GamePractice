#include "GameElemsPoolTests.hpp"
#include "Game/Logics/GameBoardElemsPool.hpp"

namespace {

const char* TEST_OBJECT_1 = "Entities/Game/Blocks/Blue.json";
const char* TEST_OBJECT_2 = "Entities/Game/Blocks/Red.json";

} // namespace

TEST_F(GameElemsPoolTests, CheckPoolDistribution) {
    auto object = createVoidObject();
    auto pool = object->addCustomLogic<GameBoardElemsPool>();
    ASSERT_TRUE(pool);
    {
        pool->ET_setElemEntity(EBoardElemType::Blue, TEST_OBJECT_1);
        pool->ET_setElemEntity(EBoardElemType::Red, TEST_OBJECT_2);

        std::vector<GameElemPoolInfo> poolsInfo;
        {
            GameElemPoolInfo info;
            info.elemType = EBoardElemType::Blue;
            info.weight = 1;
            poolsInfo.emplace_back(info);
        }
        {
            GameElemPoolInfo info;
            info.elemType = EBoardElemType::Red;
            info.weight = 2;
            poolsInfo.emplace_back(info);

        }
        pool->ET_setPoolsSetup(poolsInfo);
    }

    const int N = 600;

    int elemType[] = {0, 0};

    for(int i = 0; i < N; ++i) {
        auto elemId = pool->ET_spawnBaseRandomElem();
        EXPECT_TRUE(elemId.isValid());

        std::string name = EntityUtils::GetEntityName(elemId);
        if(name.rfind(TEST_OBJECT_1) == 0) {
            ++elemType[0];
        } else if(name.rfind(TEST_OBJECT_2) == 0) {
            ++elemType[1];
        } else {
            ASSERT_TRUE(false);
        }
    }

    int range = N / 8;
    EXPECT_GE(elemType[0], N / 3 - range);
    EXPECT_LE(elemType[0], N / 3 + range);
}

TEST_F(GameElemsPoolTests, CheckPoolRespawn) {
    auto object = createVoidObject();
    auto pool = object->addCustomLogic<GameBoardElemsPool>();
    ASSERT_TRUE(pool);
    {
        pool->ET_setElemEntity(EBoardElemType::Blue, TEST_OBJECT_1);

        std::vector<GameElemPoolInfo> poolsInfo;
        {
            GameElemPoolInfo info;
            info.elemType = EBoardElemType::Blue;
            info.weight = 1;
            poolsInfo.emplace_back(info);
        }
        pool->ET_setPoolsSetup(poolsInfo);
    }

    auto firstElemId = pool->ET_spawnBaseRandomElem();
    pool->ET_removeElem(firstElemId);

    auto secondElemId = pool->ET_spawnBaseRandomElem();
    auto thirdElemId = pool->ET_spawnBaseRandomElem();

    EXPECT_TRUE(secondElemId.isValid());
    EXPECT_TRUE(thirdElemId.isValid());

    EXPECT_EQ(firstElemId, secondElemId);
    EXPECT_NE(secondElemId, thirdElemId);
}