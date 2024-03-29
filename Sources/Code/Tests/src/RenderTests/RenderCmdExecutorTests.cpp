#include "RenderTests/RenderCmdExecutorTests.hpp"
#include "Render/src/Commands/DrawCommandExecutor.hpp"

namespace {

struct TestDrawCmd : public DrawCmd {
public:

    static bool IsVisible(const TestDrawCmd& cmd) {
        return cmd.visible;
    }

public:

    bool visible { true };
};

struct TestDrawCmdExecutor : DrawCommandExecutor<TestDrawCmd> {

    bool init() override {
        return true;
    }

    void deinit() {}

    void draw(RenderState& renderState, DrawCmdSlice& slice) {

    }
};

} // namespace

TEST_F(RenderCmdExecutorTests, CheckAddRemoveCmd) {
    RenderState renderState;

    TestDrawCmdExecutor executor;
    TestDrawCmd* cmd = static_cast<TestDrawCmd*>(executor.createCmd());
    cmd->visible = true;

    executor.preDraw(renderState);
    EXPECT_TRUE(cmd);

    {
        DrawCmdSlice slice = executor.getCmdSlice();
        slice.makeMaxSlice();

        EXPECT_EQ(slice.startIdx, 0u);
        EXPECT_EQ(slice.endIdx, 0u);
        EXPECT_EQ(slice.size, 0u);
    }

    executor.registerCmdForDraw(cmd);
    executor.preDraw(renderState);

    {
        DrawCmdSlice slice = executor.getCmdSlice();
        slice.makeMaxSlice();

        EXPECT_EQ(slice.startIdx, 0u);
        EXPECT_EQ(slice.endIdx, 1u);
        EXPECT_EQ(slice.size, 1u);
    }

    executor.destroyCmd(cmd);
    executor.preDraw(renderState);

    {
        DrawCmdSlice slice = executor.getCmdSlice();
        slice.makeMaxSlice();

        EXPECT_EQ(slice.startIdx, 0u);
        EXPECT_EQ(slice.endIdx, 0u);
        EXPECT_EQ(slice.size, 0u);
    }
}

TEST_F(RenderCmdExecutorTests, CheckCmdSorting) {
    RenderState renderState;
    TestDrawCmdExecutor executor;
    {
        TestDrawCmd* cmd = static_cast<TestDrawCmd*>(executor.createCmd());
        cmd->visible = false;
        cmd->zIndex = 3;
        executor.registerCmdForDraw(cmd);

    }
    {
        TestDrawCmd* cmd = static_cast<TestDrawCmd*>(executor.createCmd());
        cmd->visible = true;
        cmd->zIndex = 2;
        executor.registerCmdForDraw(cmd);
    }
    {
        TestDrawCmd* cmd = static_cast<TestDrawCmd*>(executor.createCmd());
        cmd->visible = true;
        cmd->zIndex = 1;
        executor.registerCmdForDraw(cmd);
    }

    executor.preDraw(renderState);

    {
        DrawCmdSlice slice = executor.getCmdSlice();
        slice.makeMaxSlice();

        EXPECT_EQ(slice.startIdx, 0u);
        EXPECT_EQ(slice.endIdx, 2u);
        EXPECT_EQ(slice.size, 2u);
    }
}
