#include "UIPartitionTests.hpp"
#include "UI/Logics/UIPartition.hpp"
#include "Game/GameObject.hpp"
#include "Core/StringFormat.hpp"
#include "Core/JSONNode.hpp"

UIPartition* UIPartitionTests::createUIPartition() {
    auto object = createVoidObject();
    std::unique_ptr<UIPartition> uiPartitionPtr(new UIPartition);
    UIPartition* uiPartition = uiPartitionPtr.get();
    object->addLogic(std::move(uiPartitionPtr));
    tempObject.push_back(std::move(object));
    return uiPartition;
}

TEST_F(UIPartitionTests, TestUIPartitionSimple) {
    auto uiPartion = createUIPartition();

    JSONNode node;
    {
        std::string jsonStr = "{\"type\":\"vert\", \"children\":[{%s}, {%s}]}";
        std::string part = "\"type\":\"box\", \"style\":{\"size\":{\"y\":0.5}}";
        jsonStr = StringFormat(jsonStr, part, part);
        node = JSONNode::ParseString(jsonStr.c_str());
    }

    ASSERT_TRUE(node);
    ASSERT_TRUE(uiPartion->serialize(node));
    ASSERT_TRUE(uiPartion->init());

    Transform tm;
    ET_SendEventReturn(tm, uiPartion->getEntityId(), &ETGameObject::ET_getTransform);
    Vec2i partitionPt = Vec2i(static_cast<int>(tm.pt.x), static_cast<int>(tm.pt.y));
    
    Vec2i renderPort;
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    EXPECT_EQ(partitionPt, renderPort / 2);
    const AABB2Di& partitionBox = uiPartion->ET_getAabb2di();
    EXPECT_EQ(partitionBox.getCenter(), partitionPt);
    EXPECT_EQ(partitionBox.getSize(), renderPort);

    std::vector<EntityId> children;
    ET_SendEventReturn(children, uiPartion->getEntityId(), &ETGameObject::ET_getChildren);

    ASSERT_EQ(children.size(), 1u);

    EntityId rootListId = children[0];
    ASSERT_TRUE(ET_IsExistNode<ETUIList>(rootListId));

    ET_SendEventReturn(children, rootListId, &ETGameObject::ET_getChildren);

    ASSERT_EQ(children.size(), 2u);
}
