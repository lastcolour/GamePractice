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
        EXPECT_EQ(obj.logic->ET_getZIndex(), 5 + (i + 1));
    }
}

TEST_F(RenderSceneTests, CheckZIndexRecurive) {
    auto root = createVoidObject();
    auto rootLogic = root->addCustomLogic<RenderSimpleLogic>();
    rootLogic->ET_setZIndex(5);

    auto rootScene = root->addCustomLogic<RenderScene>();

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

    int zIndexOffset = 3;
    rootScene->ET_addItem(zIndexOffset, objects[0].entity->getEntityId());

    for(int i = 0; i < 5; ++i) {
        auto& obj = objects[i];
        EXPECT_EQ(obj.logic->ET_getZIndex(), 5 + (i + 1) * zIndexOffset);
    }
}

TEST_F(RenderSceneTests, CheckAddHiddenChild) {
    auto root = createVoidObject();
    auto rootLogic = root->addCustomLogic<RenderSimpleLogic>();
    auto rootScene = root->addCustomLogic<RenderScene>();

    auto child = createVoidObject();
    auto childLogic = child->addCustomLogic<RenderSimpleLogic>();

    childLogic->ET_hide();

    rootScene->ET_addItem(1, child->getEntityId());

    {
        rootLogic->ET_hide();
        rootLogic->ET_show();

        EXPECT_FALSE(childLogic->ET_isVisible());
    }

    childLogic->ET_show();

    {
        rootLogic->ET_hide();
        rootLogic->ET_show();

        EXPECT_TRUE(childLogic->ET_isVisible());
    }
}

TEST_F(RenderSceneTests, CheckRecursiveRemove) {
    auto root = createVoidObject();
    root->addCustomLogic<RenderSimpleLogic>();
    auto rootScene = root->addCustomLogic<RenderScene>();

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

    EXPECT_EQ(rootScene->ET_getItemsCount(), 5);

    rootScene->ET_removeItem(objects[0].entity->getEntityId());

    EXPECT_EQ(rootScene->ET_getItemsCount(), 0);
}

TEST_F(RenderSceneTests, CheckDoNotOverrideChildRenderScene) {
    std::vector<TestRenderObject> objects;

    for(int i = 0; i < 5; ++i) {
        TestRenderObject obj;
        obj.entity = createVoidObject();
        obj.logic = obj.entity->addCustomLogic<RenderSimpleLogic>();

        if(i < 2) {
            obj.entity->addCustomLogic<RenderScene>();
        }
        if(i > 0) {
            objects.back().entity->ET_addChild(obj.entity->getEntityId());
        }

        objects.push_back(obj);
    }

    ET_SendEvent(objects[0].entity->getEntityId(),
        &ETRenderScene::ET_addItem, 1, objects[1].entity->getEntityId());
    ET_SendEvent(objects[1].entity->getEntityId(),
        &ETRenderScene::ET_addItem, 1, objects[2].entity->getEntityId());

    {
        size_t itemCount = 0;
        ET_SendEventReturn(itemCount, objects[0].entity->getEntityId(),
            &ETRenderScene::ET_getItemsCount);

        EXPECT_EQ(itemCount, 1);
    }

    {
        size_t itemCount = 0;
        ET_SendEventReturn(itemCount, objects[1].entity->getEntityId(),
            &ETRenderScene::ET_getItemsCount);

        EXPECT_EQ(itemCount, 3);
    }
}