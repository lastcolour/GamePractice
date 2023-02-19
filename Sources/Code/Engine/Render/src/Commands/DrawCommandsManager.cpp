#include "Commands/DrawCommandsManager.hpp"
#include "Commands/DrawColoredQuadExecutor.hpp"
#include "Commands/DrawTextExecutor.hpp"
#include "Commands/DrawParticlesExecutor.hpp"
#include "Commands/DrawTexturedQuadExecutor.hpp"
#include "Commands/DrawBlurExecutor.hpp"
#include "Commands/DrawNinePatchExecutor.hpp"
#include "Render/ETRenderTickManager.hpp"
#include "Render/ETRenderCamera.hpp"

DrawCommandsManager::DrawCommandsManager() :
    clearColor(0.f, 0.f, 0.f, 1.f) {
}

DrawCommandsManager::~DrawCommandsManager() {
}

bool DrawCommandsManager::init() {

    cmdExecutorStates.resize(static_cast<size_t>(EDrawCmdType::COUNT));

    cmdExecutorStates[static_cast<size_t>(EDrawCmdType::Text)]
        .executor.reset(new DrawTextExecutor);

    cmdExecutorStates[static_cast<size_t>(EDrawCmdType::Quad)]
        .executor.reset(new DrawColoredQuadExecutor);

    cmdExecutorStates[static_cast<size_t>(EDrawCmdType::TexturedQuad)]
        .executor.reset(new DrawTexturedQuadExecutor);

    cmdExecutorStates[static_cast<size_t>(EDrawCmdType::Particles)]
        .executor.reset(new DrawParticlesExecutor);

    cmdExecutorStates[static_cast<size_t>(EDrawCmdType::Blur)]
        .executor.reset(new DrawBlurExecutor);

    cmdExecutorStates[static_cast<size_t>(EDrawCmdType::NinePatch)]
        .executor.reset(new DrawNinePatchExecutor);

    debugCmdExecutor.reset(new DebugDrawExecutor);

    ETNode<ETDrawCommandsManager>::connect(getEntityId());
    ETNode<ETRenderContextEvents>::connect(getEntityId());

    if(RenderUtils::IsOpenGLContextExists()) {
        ET_onContextCreated();
    }

    return true;
}

void DrawCommandsManager::deinit() {
}

void DrawCommandsManager::ET_onContextCreated() {
    if(!renderState.init()) {
        LogError("[DrawCommandsManager::ET_onContextCreated] Can't init render state");
        return;
    }
    for(size_t i = 0, sz = cmdExecutorStates.size(); i < sz; ++i) {
        auto& executor = cmdExecutorStates[i].executor;
        if(!executor->init()) {
            LogError("[DrawCommandsManager::ET_onContextCreated] Can't init cmd executor: '%s'",
                DrawCmdUtils::GetNameOfDrawCmdType(static_cast<EDrawCmdType>(i)));
            cmdExecutorStates[i].inited = false;
        } else {
            executor->addEvent(EDrawCmdEventType::UpdateVertexData);
            cmdExecutorStates[i].inited = true;
        }
    }
    if(!debugCmdExecutor->init()) {
        LogError("[DrawCommandsManager::ET_onContextCreated] Can't init debug cmd executor");
        return;
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
}

void DrawCommandsManager::ET_onContextDestroyed() {
}

DrawCmd* DrawCommandsManager::ET_createDrawCmd(EDrawCmdType cmdType) {
    auto executor = cmdExecutorStates[static_cast<size_t>(cmdType)].executor.get();
    if(!executor) {
        LogError("[RenderCommandsManager::ET_createDrawCmd] Can't find an executor for cmd type: '%d'", cmdType);
        return nullptr;
    }
    auto cmd = executor->createCmd();
    if(!cmd) {
        LogError("[RenderCommandsManager::ET_createDrawCmd] Can't create cmd of type: '%d'", cmdType);
        return nullptr;
    }

    return cmd;
}

void DrawCommandsManager::ET_scheduleDrawCmdEvent(std::function<void(BaseDrawCommandExectuor*)>&& func, EDrawCmdType cmdType) {
    auto& ex = cmdExecutorStates[static_cast<size_t>(cmdType)].executor;
    func(ex.get());
}

void DrawCommandsManager::ET_preRender() {
    debugCmdExecutor->preRender();
}

void DrawCommandsManager::ET_renderToDefaultFBO() {
    render(EDrawContentFilter::None);
    renderState.copyFBOtoDefaultFBO(*renderState.mainFBO);
}

void DrawCommandsManager::ET_renderToBuffer(void* out, EDrawContentFilter filter) {
    render(filter);
    RenderUtils::ReadFramebufferToBuffer(*renderState.mainFBO, out);
}

void DrawCommandsManager::render(EDrawContentFilter filter) {
    ET_SendEvent(&ETRenderTickManager::ET_fetchDeltaT);

    ET_SendEventReturn(renderState.proj2dMat, &ETRenderCamera::ET_getProj2DMat4);

    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETRenderCamera::ET_getRenderPort);

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[DrawCommandsManager::render] GL has render start error: %s", errStr);
        return;
    }

    renderState.mainFBO->bind();
    if(!renderState.mainFBO->resize(viewport)) {
        LogError("[DrawCommandsManager::render] Can't resize framebuffer to viewport size: [%d x %d]", viewport.x, viewport.y);
        return;
    }
    renderState.mainFBO->clear();

    for(size_t i = 0, sz = cmdExecutorStates.size(); i < sz; ++i) {
        auto& exState = cmdExecutorStates[i];
        if(!exState.inited) {
            continue;
        }

        exState.executor->preDraw(renderState);

        if(auto errStr = RenderUtils::GetGLError()) {
            LogError("[DrawCommandsManager::render] GL has preDraw() error for an executor: %s. Error: %s",
                DrawCmdUtils::GetNameOfDrawCmdType(static_cast<EDrawCmdType>(i)), errStr);
        }

        exState.executor->clearEvents();
        exState.slice = exState.executor->getCmdSlice();
        exState.slice.advance();
    }

    while(true) {
        CmdExecutorState* firstMin = nullptr;
        CmdExecutorState* secondMin = nullptr;

        int totalExecutors = 0;
        for(auto& exState : cmdExecutorStates) {
            if(!exState.slice.empty()) {
               totalExecutors += 1;
               if(!firstMin || exState.slice < firstMin->slice) {
                   secondMin = firstMin;
                   firstMin = &exState;
               } else if (!secondMin || exState.slice < secondMin->slice) {
                    secondMin = &exState;
               }
            }
        }
        if(!firstMin) {
           break;
        }
        if(totalExecutors > 1) {
            firstMin->slice.tryToGrowUpTo(secondMin->slice);
            firstMin->executor->draw(renderState, firstMin->slice);
            firstMin->slice.advance();
        } else {
            firstMin->slice.makeMaxSlice();
            firstMin->executor->draw(renderState, firstMin->slice);
            break;
        }
    }

    if(filter != EDrawContentFilter::NoDebugInfo) {
        debugCmdExecutor->draw(renderState);
    }

    renderState.mainFBO->unbind();

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[DrawCommandsManager::render] GL has render end error: %s", errStr);
    }
}