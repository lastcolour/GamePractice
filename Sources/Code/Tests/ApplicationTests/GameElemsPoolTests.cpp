#include "GameElemsPoolTests.hpp"
#include "Game/Logics/GameBoardElemsPool.hpp"

namespace {

const char* TEST_OBJECT_1 = "Game/Blocks/Blue.json";
const char* TEST_OBJECT_2 = "Game/Blocks/Red.json";

} // namespace

TEST_F(GameElemsPoolTests, CheckPoolDistribution) {
    auto object = createVoidObject();
    auto pool = new GameBoardElemsPool();
    object->addCustomLogic(std::unique_ptr<EntityLogic>(pool));
    {
        ElementDescriptor elemDescr;
        elemDescr.weight = 1;
        elemDescr.object = TEST_OBJECT_1;
        pool->ET_addElemsToSpawn(elemDescr);
    }
    {
        ElementDescriptor elemDescr;
        elemDescr.weight = 2;
        elemDescr.object = TEST_OBJECT_2;
        pool->ET_addElemsToSpawn(elemDescr);
    }

    const int N = 600;

    int elemType[] = {0, 0};

    for(int i = 0; i < N; ++i) {
        auto elemId = pool->ET_spawnElem();
        EXPECT_TRUE(elemId.isValid());

        std::string name = EntityUtils::GetEntityName(elemId);
        if(name == TEST_OBJECT_1) {
            ++elemType[0];
        } else if(name == TEST_OBJECT_2) {
            ++elemType[1];
        } else {
            ASSERT_TRUE(false);
        }
    }

    int range = N / 8;
    EXPECT_GE(elemType[0], N / 3 - range);
    EXPECT_LE(elemType[0], N / 3 + range);
}