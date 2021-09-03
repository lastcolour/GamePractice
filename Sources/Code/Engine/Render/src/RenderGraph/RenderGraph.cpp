#include "RenderGraph/RenderGraph.hpp"
#include "Nodes/Node.hpp"
#include "RenderGraph/RenderContext.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderFramebuffer.hpp"
#include "Render/ETDebugRender.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderUtils.hpp"
#include "Platform/OpenGL.hpp"
#include "Render/ETRenderCamera.hpp"

#include <cassert>

namespace {

const int EXTRA_FBOS_COUNT = 2;

} // namespace

RenderGraph::RenderGraph() :
    clearColor(0, 0, 0, 1.f),
    drawFilter(DrawContentFilter::None),
    needReorder(false) {
}

RenderGraph::~RenderGraph() {
}

void RenderGraph::init() {
    ET_SendEventReturn(mainFBO, &ETRenderTextureManager::ET_createFramebuffer, EFramebufferType::Color_Depth_Stencil);
    if(!mainFBO) {
        LogError("[RenderGraph::init] Can't create main framebuffer");
        assert(false && "Can't create framebuffer");
    }

    mainFBO->color0.bind();
    mainFBO->color0.setLerpType(ETextureLerpType::Linear, ETextureLerpType::Linear);
    mainFBO->color0.setWrapType(ETextureWrapType::ClamToEdge, ETextureWrapType::ClamToEdge);
    mainFBO->color0.unbind();

    for(int i = 0; i < EXTRA_FBOS_COUNT; ++i) {
        std::shared_ptr<RenderFramebuffer> extraFBO;
        ET_SendEventReturn(extraFBO, &ETRenderTextureManager::ET_createFramebuffer, EFramebufferType::Color);
        if(!extraFBO) {
            LogError("[RenderGraph::init] Can't create extra framebuffer");
            assert(false && "Can't create extra framebuffer");
        }

        extraFBO->color0.bind();
        extraFBO->color0.setLerpType(ETextureLerpType::Linear, ETextureLerpType::Linear);
        extraFBO->color0.setWrapType(ETextureWrapType::ClamToEdge, ETextureWrapType::ClamToEdge);
        extraFBO->color0.unbind();

        extraFBOs.push_back(extraFBO);
    }

    ctx.mainFBO = mainFBO.get();
    for(auto& fbo : extraFBOs) {
        ctx.exraFBOs.push_back(fbo.get());
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

    if(!fboCopyLogic.init()) {
        LogError("[RenderGraph::init] Can't init FBO copy logic");
    }
}

bool RenderGraph::startFrame() {
    prepareNodes();

    ET_SendEventReturn(ctx.proj2dMat, &ETRenderCamera::ET_getProj2DMat4);

    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETRenderCamera::ET_getRenderPort);

    mainFBO->bind();
    if(!mainFBO->resize(viewport)) {
        mainFBO->unbind();
        LogError("[RenderGraph::startFrame] Can't resize framebuffer to size: %dx%d", viewport.x, viewport.y);
        return false;
    }
    mainFBO->clear();
    return true;
}

void RenderGraph::endFrame() {
    ctx.setBlending(BlendMode{BlendType::NONE, BlendType::NONE});
    ctx.setStencilState(StencilWirteReadData());
    mainFBO->unbind();
}

void RenderGraph::prepareNodes() {
    if(!needReorder) {
        return;
    }
    std::sort(children.begin(), children.end(), [](Node* first, Node* second){
        return first->getZIndex() < second->getZIndex();
    });
    needReorder = false;
}

void RenderGraph::addChild(Node* node) {
    needReorder = true;
    node->setRenderGraph(this);
    children.push_back(node);
}

void RenderGraph::removeChild(Node* node) {
    auto it = std::find(children.begin(), children.end(), node);
    if(it != children.end()) {
        std::swap(*it, children.back());
        children.pop_back();
        needReorder = true;
    } else {
        assert(false && "Can't find child to remove");
    }
}

void RenderGraph::setNeedReorderNodes() {
    needReorder = true;
}

void RenderGraph::drawToFBO(DrawContentFilter filter) {
    if(!startFrame()) {
        return;
    }
    for(auto node : children) {
        if(node->canRender()) {
            node->render(ctx);
        }
    }
    if(filter != DrawContentFilter::NoDebugInfo) {
        ET_SendEvent(&ETDebugRender::ET_update, ctx);
    }
    endFrame();
}

void RenderGraph::render() {
    drawToFBO(drawFilter);
    fboCopyLogic.copyToDefault(*mainFBO);
}

void RenderGraph::renderToBuffer(void* outBuffer, DrawContentFilter filter) {
    drawToFBO(filter);
    RenderUtils::ReadFramebufferToBuffer(*mainFBO, outBuffer);
}