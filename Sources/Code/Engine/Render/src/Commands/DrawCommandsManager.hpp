#ifndef __DRAW_COMMANDS_MANAGER_HPP__
#define __DRAW_COMMANDS_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Commands/DrawCommandExecutor.hpp"
#include "Commands/DebugDrawExecutor.hpp"
#include "Render/ETRenderInterfaces.hpp"

class DrawCommandsManager : public SystemLogic,
    public ETNode<ETDrawCommandsManager>,
    public ETNode<ETRenderContextEvents> {
public:

    DrawCommandsManager();
    virtual ~DrawCommandsManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETDrawCommandsManager
    DrawCmd* ET_createDrawCmd(EDrawCmdType cmdType) override;
    void ET_scheduleDrawCmdEvent(std::function<void(BaseDrawCommandExectuor*)>&& func, EDrawCmdType cmdType) override;
    void ET_preRender() override;
    void ET_renderToDefaultFBO() override;
    void ET_renderToBuffer(void* out, EDrawContentFilter filter) override;

    // ETRenderContextEvents
    void ET_onContextCreated() override;
    void ET_onContextDestroyed() override;

private:

    void render(EDrawContentFilter filter);

private:

    class CmdExecutorState {
    public:

        CmdExecutorState() :
            inited(false) {}

        CmdExecutorState(CmdExecutorState&& other) :
            slice(other.slice),
            executor(std::move(other.executor)),
            inited(other.inited) {
            other.inited = false;
        }

        CmdExecutorState& operator=(CmdExecutorState&& other) {
            if(this != &other) {
                slice = other.slice;
                executor = std::move(other.executor);
                inited = other.inited;

                other.inited = false;
            }
            return *this;
        }

    public:

        DrawCmdSlice slice;
        std::unique_ptr<BaseDrawCommandExectuor> executor;
        bool inited;
    };

private:

    ColorF clearColor;
    RenderState renderState;
    std::vector<DrawCmd*> pendingQueueCmd;
    std::vector<CmdExecutorState> cmdExecutorStates;
    std::unique_ptr<DebugDrawExecutor> debugCmdExecutor;
};

#endif /* __DRAW_COMMANDS_MANAGER_HPP__ */