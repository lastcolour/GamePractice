#include "RenderSceneTests.hpp"
#include "Logics/RenderScene.hpp"
#include "Logics/RenderSimpleLogic.hpp"

namespace {

struct TestRenderObject {
    Entity* entity{nullptr};
    RenderSimpleLogic* logic{nullptr};
};

} // namespace

TEST_F(RenderSceneTests, CheckZIndex) {
    auto root = createVoidObject();
    auto rootLogic = root->addCustomLogic<RenderSimpleLogic>();
    rootLogic->ET_setZIndex(5);

    auto rootScene = root->addCustomLogic<RenderScene>();

    {
        auto params = rootScene->ET_getParams();
        params.zIndexStep = 10;
        rootScene->ET_setParams(params);
    }

    std::vector<TestRenderObject> objects;

    for(int i = 0; i < 5; ++i) {
        TestRenderObject object;
        object.entity = createVoidObject();
        object.logic = object.entity->addCustomLogic<RenderSimpleLogic>();
        objects.push_back(object);

        rootScene->ET_addItem(i + 1, object.entity->getEntityId());
    }

    for(int i = 0; i < 5; ++i) {
        auto& obj = objects[i];
        EXPECT_EQ(obj.logic->ET_getZIndex(), 5 + (i + 1) * 10);
    }
}

TEST_F(RenderSceneTests, CheckZIndexRecurive) {
    auto root = createVoidObject();
    auto rootLogic = root->addCustomLogic<RenderSimpleLogic>();
    rootLogic->ET_setZIndex(5);

    auto rootScene = root->addCustomLogic<RenderScene>();

    {
        auto params = rootScene->ET_getParams();
        params.zIndexStep = 10;
        rootScene->ET_setParams(params);
    }

    std::vector<TestRenderObject> objects;

    for(int i = 0; i < 5; ++i) {
        TestRenderObject obj;
        obj.entity = createVoidObject();
        obj.logic = obj.entity->addCustomLogic<RenderSimpleLogic>();

        if(i > 0) {
            objects.back().entity->ET_addChild(obj.entity->getEntityId());
        }

        objects.push_back(obj);
    }

    rootScene->ET_addItem(1, objects[0].entity->getEntityId());

    for(int i = 0; i < 5; ++i) {
        auto& obj = objects[i];
        EXPECT_EQ(obj.logic->ET_getZIndex(), 5 + (i + 1) * 10);
    }
}