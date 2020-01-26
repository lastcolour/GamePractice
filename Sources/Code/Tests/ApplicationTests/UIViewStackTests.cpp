#include "UIViewStackTests.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "TestUtils/UITestUtils.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"

namespace {

const char* TEST_VIEW_1 = "UI/PauseView/Root.json";
const char* TEST_VIEW_2 = "UI/ExitView/Root.json";
const char* TEST_VIEW_3 = "UI/AboutView/Root.json";

enum class StackEventType {
        StartPush = 0,
        FinishPush,
        StartPop,
        FinishPop
};

class ViewStackEventHitoryLogic : public EntityLogic,
    public ETNode<ETUIViewStackEvents> {
public:

    ViewStackEventHitoryLogic() = default;
    virtual ~ViewStackEventHitoryLogic() = default;

    // EntityLogic
    bool serialize(const JSONNode& node) {
        return true;
    }

    bool init() {
        ETNode<ETUIViewStackEvents>::connect(getEntityId());
        return true;
    }

    // ETUIViewStackEvents
    void ET_onViewStartPush(EntityId viewId) override {
        eventsHistory.push_back({StackEventType::StartPush, viewId});
    }
    void ET_onViewFinishPush(EntityId viewId) override {
        eventsHistory.push_back({StackEventType::FinishPush, viewId});
    }
    void ET_onViewStartPop(EntityId viewId) override {
        eventsHistory.push_back({StackEventType::StartPop, viewId});
    }
    void ET_onViewFinishPop(EntityId viewId) override {
        eventsHistory.push_back({StackEventType::FinishPop, viewId});
    }

public:

    struct StackEvent {
        StackEventType eventType;
        EntityId viewId;
    };

public:

    std::vector<StackEvent> eventsHistory;
};

}; // namespace

void UIViewStackTests::TearDown() {
    ET_SendEvent(&ETUIViewStack::ET_forceReset);
    ConsoleAppTests::TearDown();
}

TEST_F(UIViewStackTests, CheckPushPushPop) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_popView);

    CheckExpectedView(TEST_VIEW_1);
}

TEST_F(UIViewStackTests, CheckPushPushPopPush) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_popView);

    WaitViewSwitchEnd();

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_3);

    CheckExpectedView(TEST_VIEW_3);
}

TEST_F(UIViewStackTests, CheckPushPushPopPushTheSame) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_popView);

    WaitViewSwitchEnd();

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);

    CheckExpectedView(TEST_VIEW_2);
}

TEST_F(UIViewStackTests, CheckTriplePush) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_3);

    CheckExpectedView(TEST_VIEW_2);

    WaitViewSwitchEnd();

    CheckExpectedView(TEST_VIEW_3);
}

TEST_F(UIViewStackTests, CheckPushEvents) {
    auto object = createObjectAndLogic<ViewStackEventHitoryLogic>();
    ASSERT_TRUE(object->init());

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);

    ASSERT_EQ(object->eventsHistory.size(), 2);
    ASSERT_EQ(object->eventsHistory[0].eventType, StackEventType::StartPush);
    ASSERT_EQ(object->eventsHistory[1].eventType, StackEventType::FinishPush);
}

TEST_F(UIViewStackTests, CheckDoublePushEvents) {
    auto object = createObjectAndLogic<ViewStackEventHitoryLogic>();
    ASSERT_TRUE(object->init());

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);

    ASSERT_EQ(object->eventsHistory.size(), 3);
    ASSERT_EQ(object->eventsHistory[0].eventType, StackEventType::StartPush);
    ASSERT_EQ(object->eventsHistory[1].eventType, StackEventType::FinishPush);
    ASSERT_EQ(object->eventsHistory[2].eventType, StackEventType::StartPush);

    WaitViewSwitchEnd();

    ASSERT_EQ(object->eventsHistory.size(), 4);
    ASSERT_EQ(object->eventsHistory[3].eventType, StackEventType::FinishPush);
}

TEST_F(UIViewStackTests, CheckTriplePushEvents) {
    auto object = createObjectAndLogic<ViewStackEventHitoryLogic>();
    ASSERT_TRUE(object->init());

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_3);

    WaitViewSwitchEnd();

    ASSERT_EQ(object->eventsHistory.size(), 5);

    WaitViewSwitchEnd();

    ASSERT_EQ(object->eventsHistory.size(), 6);

    ASSERT_EQ(object->eventsHistory[4].eventType, StackEventType::StartPush);
    ASSERT_EQ(object->eventsHistory[5].eventType, StackEventType::FinishPush);
}

TEST_F(UIViewStackTests, CheckPopWithOneViewEvents) {
    auto object = createObjectAndLogic<ViewStackEventHitoryLogic>();
    ASSERT_TRUE(object->init());

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_popView);

    ASSERT_EQ(object->eventsHistory.size(), 2);

    ASSERT_EQ(object->eventsHistory[0].eventType, StackEventType::StartPush);
    ASSERT_EQ(object->eventsHistory[1].eventType, StackEventType::FinishPush);
}

TEST_F(UIViewStackTests, CheckPopWihTwoViewEvents) {
    auto object = createObjectAndLogic<ViewStackEventHitoryLogic>();
    ASSERT_TRUE(object->init());

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);

    WaitViewSwitchEnd();

    ET_SendEvent(&ETUIViewStack::ET_popView);

    ASSERT_EQ(object->eventsHistory.size(), 5);
    ASSERT_EQ(object->eventsHistory[4].eventType, StackEventType::StartPop);

    WaitViewSwitchEnd();

    ASSERT_EQ(object->eventsHistory.size(), 6);

    ASSERT_EQ(object->eventsHistory[5].eventType, StackEventType::FinishPop);
}

TEST_F(UIViewStackTests, CheckDoublePopEvents) {
    auto object = createObjectAndLogic<ViewStackEventHitoryLogic>();
    ASSERT_TRUE(object->init());

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_3);

    WaitViewSwitchEnd();
    WaitViewSwitchEnd();

    ET_SendEvent(&ETUIViewStack::ET_popView);
    ET_SendEvent(&ETUIViewStack::ET_popView);

    WaitViewSwitchEnd();

    ASSERT_EQ(object->eventsHistory.size(), 9);
    ASSERT_EQ(object->eventsHistory[8].eventType, StackEventType::StartPop);

    WaitViewSwitchEnd();

    ASSERT_EQ(object->eventsHistory.size(), 10);
    ASSERT_EQ(object->eventsHistory[9].eventType, StackEventType::FinishPop);
}

TEST_F(UIViewStackTests, CheckInterruptPopEvents) {
    auto object = createObjectAndLogic<ViewStackEventHitoryLogic>();
    ASSERT_TRUE(object->init());

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);

    ET_SendEvent(&ETUIViewStack::ET_popView);

    WaitViewSwitchEnd();

    ASSERT_EQ(object->eventsHistory.size(), 5);
    ASSERT_EQ(object->eventsHistory[2].eventType, StackEventType::StartPush);
    ASSERT_EQ(object->eventsHistory[3].eventType, StackEventType::StartPop);
    ASSERT_EQ(object->eventsHistory[4].eventType, StackEventType::FinishPop);
}

TEST_F(UIViewStackTests, CheckInterruptPushEvents) {
    auto object = createObjectAndLogic<ViewStackEventHitoryLogic>();
    ASSERT_TRUE(object->init());

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);

    WaitViewSwitchEnd();

    ET_SendEvent(&ETUIViewStack::ET_popView);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);

    WaitViewSwitchEnd();

    ASSERT_EQ(object->eventsHistory.size(), 7);
    ASSERT_EQ(object->eventsHistory[4].eventType, StackEventType::StartPop);
    ASSERT_EQ(object->eventsHistory[5].eventType, StackEventType::StartPush);
    ASSERT_EQ(object->eventsHistory[6].eventType, StackEventType::FinishPush);
}

TEST_F(UIViewStackTests, CheckStackStateAfterResize) {
    auto historyObj = createObjectAndLogic<ViewStackEventHitoryLogic>();

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);

    WaitViewSwitchEnd();

    ET_SendEvent(&ETRenderEvents::ET_onRenderPortResized);
}

TEST_F(UIViewStackTests, CheckDrawPriorities) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);

    auto firstViewId = InvalidEntityId;
    ET_SendEventReturn(firstViewId, &ETUIViewStack::ET_getActiveViewId);

    int firstZIindex = 0;
    ET_SendEventReturn(firstZIindex, firstViewId, &ETUIBox::ET_getZIndex);

    EXPECT_GE(firstZIindex, 0);

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    WaitViewSwitchEnd();

    auto secondViewId = InvalidEntityId;
    ET_SendEventReturn(secondViewId, &ETUIViewStack::ET_getActiveViewId);

    int secondZIndex = 0;
    ET_SendEventReturn(secondZIndex, secondViewId, &ETUIBox::ET_getZIndex);

    int firstViewChildDepth = 0;
    ET_SendEventReturn(firstViewChildDepth, firstViewId, &ETEntity::ET_getMaxChildrenDepth);

    EXPECT_GT(secondZIndex, firstZIindex + firstViewChildDepth);
}

TEST_F(UIViewStackTests, CheckFinishPushAfterResize) {
    auto object = createObjectAndLogic<ViewStackEventHitoryLogic>();
    ASSERT_TRUE(object->init());

    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);

    EXPECT_EQ(object->eventsHistory.size(), 3);
    EXPECT_EQ(object->eventsHistory[0].eventType, StackEventType::StartPush);
    EXPECT_EQ(object->eventsHistory[1].eventType, StackEventType::FinishPush);
    EXPECT_EQ(object->eventsHistory[2].eventType, StackEventType::StartPush);

    ET_SendEvent(&ETRenderEvents::ET_onRenderPortResized);

    EXPECT_EQ(object->eventsHistory.size(), 4);
    EXPECT_EQ(object->eventsHistory[3].eventType, StackEventType::FinishPush);
}

TEST_F(UIViewStackTests, CheckFinishPopAfterResize) {
}