#include "UIReAlignManagerTests.hpp"
#include "Logics/UIBox.hpp"
#include "Logics/UILayout.hpp"
#include "Logics/UILayoutBox.hpp"
#include "UI/ETUIView.hpp"

namespace {

struct RealignEventListener : public EntityLogic,
    ETNode<ETUIElemAligner> {

    static int ReAlignCallCount;

public:

    void init() override {
        ETNode<ETUIElemAligner>::connect(getEntityId());
    }

    void deinit() override {}

    void ET_reAlign() {
        ++ReAlignCallCount;
    }
};


int RealignEventListener::ReAlignCallCount = 0;

void SetupChild(Entity* entity) {
    UIBoxStyle style;
    style.height = 100.f;
    style.width = 200.f;
    style.heightInv = UIBoxSizeInvariant::Grid;
    style.widthInv = UIBoxSizeInvariant::Grid;

    auto box = entity->addCustomLogic<UIBox>();
    box->ET_setStyle(style);
}

void SetupParent(Entity* entity) {
    entity->addCustomLogic<UILayoutBox>();
    entity->addCustomLogic<RealignEventListener>();
    entity->addCustomLogic<UILayout>();
}

void AddChildToParent(Entity* childEnt, Entity* parentEnt) {
    parentEnt->ET_addChild(childEnt->getEntityId());
    ET_SendEvent(parentEnt->getEntityId(), &ETUILayout::ET_addItem, childEnt->getEntityId());
}

void MakeUIElemDirty(Entity* entity) {
    UIBoxStyle boxStyle;
    ET_SendEventReturn(boxStyle, entity->getEntityId(), &ETUIBox::ET_getStyle);
    ET_SendEvent(entity->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);

    UILayoutStyle layoutStyle;
    ET_SendEventReturn(layoutStyle, entity->getEntityId(), &ETUILayout::ET_getStyle);
    ET_SendEvent(entity->getEntityId(), &ETUILayout::ET_setStyle, layoutStyle);
}

} // namespace

void UIReAlignManagerTests::SetUpTestCase() {
    CreateTestApp(ETestAppModules::CheckUI);
}

void UIReAlignManagerTests::SetUp() {
    RealignEventListener::ReAlignCallCount = 0;
}

TEST_F(UIReAlignManagerTests, TestSimple) {
    auto childEnt = createVoidObject();
    SetupChild(childEnt);

    auto parentEnt = createVoidObject();
    SetupParent(parentEnt);

    AddChildToParent(childEnt, parentEnt);

    ET_SendEvent(&ETUIReAlignManager::ET_doReAlign);

    EXPECT_EQ(RealignEventListener::ReAlignCallCount, 1);
}

TEST_F(UIReAlignManagerTests, CheckChain) {
    std::vector<Entity*> entites;
    int N = 5;
    Entity* prevEnt = nullptr;
    for(int i = 0; i < N; ++i) {
        if(prevEnt) {
            auto newEnt = createVoidObject();
            SetupParent(newEnt);
            AddChildToParent(prevEnt, newEnt);
            prevEnt = newEnt;
        } else {
            prevEnt = createVoidObject();
            SetupChild(prevEnt);
        }

        entites.push_back(prevEnt);
    }

    ET_SendEvent(&ETUIReAlignManager::ET_doReAlign);

    EXPECT_EQ(RealignEventListener::ReAlignCallCount, N - 1);

    RealignEventListener::ReAlignCallCount = 0;

    MakeUIElemDirty(entites[3]);

    ET_SendEvent(&ETUIReAlignManager::ET_doReAlign);

    EXPECT_EQ(RealignEventListener::ReAlignCallCount, N - 3);
}

TEST_F(UIReAlignManagerTests, CheckGraph) {
    auto leftParent = createVoidObject();
    SetupParent(leftParent);
    {
        auto leftChild = createVoidObject();
        SetupChild(leftChild);
        auto rightChild = createVoidObject();
        SetupChild(rightChild);

        AddChildToParent(leftChild, leftParent);
        AddChildToParent(rightChild, leftParent);
    }

    auto rightParent = createVoidObject();
    SetupParent(rightParent);
    {
        auto leftChild = createVoidObject();
        SetupChild(leftChild);
        auto rightChild = createVoidObject();
        SetupChild(rightChild);

        AddChildToParent(leftChild, rightParent);
        AddChildToParent(rightChild, rightParent);
    }

    auto rootParent = createVoidObject();
    SetupParent(rootParent);

    AddChildToParent(leftParent, rootParent);
    AddChildToParent(rightParent, rootParent);

    ET_SendEvent(&ETUIReAlignManager::ET_doReAlign);

    EXPECT_EQ(RealignEventListener::ReAlignCallCount, 3);
}
