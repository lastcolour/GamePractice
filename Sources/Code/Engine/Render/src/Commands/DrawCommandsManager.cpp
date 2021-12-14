#include "Commands/DrawCommandsManager.hpp"
#include "Commands/DrawColoredQuadExecutor.hpp"
#include "Commands/DrawTextExecutor.hpp"
#include "Commands/DrawParticlesExecutor.hpp"
#include "Commands/DrawTexturedQuadExecutor.hpp"
#include "Commands/DrawBlurExecutor.hpp"
#include "Render/ETRenderCamera.hpp"
#include "RenderUtils.hpp"

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

    ETNode<ETDrawCommandsManager>::connect(getEntityId());
    ETNode<ETRenderContextEvents>::connect(getEntityId());
    return true;
}

void DrawCommandsManager::deinit() {
}

void DrawCommandsManager::ET_onContextCreated() {
    if(!renderState.init()) {
        LogError("[DrawCommandsManager::ET_onContextCreated] Can't init render state");
        return;
    }
    for(auto& exState : cmdExecutorStates) {
        if(!exState.executor->init()) {
            LogError("[DrawCommandsManager::ET_onContextCreated] Can't init cmd executor");
            return;
        }
        exState.executor->addEvent(EDrawCmdEventType::UpdateVertexData);
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
        LogError("[RenderCommandsManager::ET_createDrawCmd] Can't create cmd of type: '%d'", cmdType);
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

void DrawCommandsManager::ET_renderToDefaultFBO() {
    render(EDrawContentFilter::None);
    renderState.copyFBOtoDefaultFBO(*renderState.mainFBO);
}

void DrawCommandsManager::ET_renderToBuffer(void* out, EDrawContentFilter filter) {
    render(filter);
    RenderUtils::ReadFramebufferToBuffer(*renderState.mainFBO, out);
}

void DrawCommandsManager::render(EDrawContentFilter filter) {
    ET_SendEventReturn(renderState.proj2dMat, &ETRenderCamera::ET_getProj2DMat4);

    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETRenderCamera::ET_getRenderPort);

    if(!renderState.mainFBO->resize(viewport)) {
        LogError("[DrawCommandsManager::render] Can't resize framebuffer to size: %dx%d", viewport.x, viewport.y);
        return;
    }
    renderState.mainFBO->bind();
    renderState.mainFBO->clear();

    for(auto& exState : cmdExecutorStates) {
        exState.executor->preDraw();
        exState.executor->clearEvents();
        exState.slice = exState.executor->getCmdSlice();
        exState.slice.advance();
    }

    while(true) {
        BaseDrawCommandExectuor* firstMinEx = nullptr;
        auto firstMinSlice = DrawCmdSlice();

        BaseDrawCommandExectuor* secondMinEx = nullptr;
        auto secondMinSlice = DrawCmdSlice();

        int totalExecutors = 0;
        for(auto& exState : cmdExecutorStates) {
            auto& slice = exState.slice;
            if(!slice.empty()) {
               totalExecutors += 1;
               if(slice < firstMinSlice) {
                   secondMinSlice = firstMinSlice;
                   secondMinEx = firstMinEx;

                   firstMinSlice = slice;
                   firstMinEx = exState.executor.get();
               }
            }
        }
        if(!firstMinSlice.empty()) {
           break;
        }
        if(totalExecutors > 1) {
            firstMinSlice.tryToGrowUpTo(secondMinSlice);
            firstMinEx->draw(renderState, firstMinSlice);
            firstMinSlice.advance();
        } else {
            firstMinSlice.makeMaxSlice();
            firstMinEx->draw(renderState, firstMinSlice);
            break;
        }
    }

    if(filter != EDrawContentFilter::NoDebugInfo) {
        debugCmdExecutor->draw(renderState);
    }
}

void DrawCommandsManager::ET_updateEmitters(float dt) {
    auto executor =  static_cast<DrawParticlesExecutor*>(
        cmdExecutorStates[static_cast<size_t>(EDrawCmdType::Particles)].executor.get());
    executor->update(dt);
}