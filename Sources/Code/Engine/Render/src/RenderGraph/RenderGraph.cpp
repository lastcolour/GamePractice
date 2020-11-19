#include "RenderGraph/RenderGraph.hpp"
#include "Nodes/Node.hpp"
#include "RenderContext.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderFramebuffer.hpp"
#include "Render/ETDebugRender.hpp"
#include "Render/ETRenderManager.hpp"
#include "Core/ETLogger.hpp"
#include "RenderUtils.hpp"
#include "Platform/OpenGL.hpp"
#include "Render/ETRenderCamera.hpp"

#include <algorithm>
#include <cassert>

namespace {

const int EXTRA_FBOS_COUNT = 2;

} // namespace

RenderGraph::RenderGraph() :
    needReorder(false),
    clearColor(0, 0, 0, 1.f) {
}

RenderGraph::~RenderGraph() {
    for(auto node : children) {
        node->setRenderGraph(nullptr);
    }
}

void RenderGraph::init() {
    ET_SendEventReturn(mainFBO, &ETRenderTextureManger::ET_createFramebuffer);
    if(!mainFBO) {
        LogError("[RenderGraph::init] Can't create main framebuffer");
        assert(false && "Can't create framebuffer");
    }

    mainFBO->texture.bind();
    mainFBO->texture.setPixelLerpType(TexLerpType::Linear, TexLerpType::Linear);
    mainFBO->texture.setPixelWrapType(TexWrapType::ClamToEdge, TexWrapType::ClamToEdge);
    mainFBO->texture.unbind();

    for(int i = 0; i < EXTRA_FBOS_COUNT; ++i) {
        std::shared_ptr<RenderFramebuffer> extraFBO;
        ET_SendEventReturn(extraFBO, &ETRenderTextureManger::ET_createFramebuffer);
        if(!extraFBO) {
            LogError("[RenderGraph::init] Can't create extra framebuffer");
            assert(false && "Can't create extra framebuffer");
        }

        extraFBO->texture.bind();
        extraFBO->texture.setPixelLerpType(TexLerpType::Linear, TexLerpType::Linear);
        extraFBO->texture.setPixelWrapType(TexWrapType::ClamToEdge, TexWrapType::ClamToEdge);
        extraFBO->texture.unbind();

        extraFBOs.push_back(extraFBO);
    }

    if(!drawFrameNode.init()) {
        LogError("[RenderGraph::init] Can't init draw frame node");
        assert(false && "Can't init draw frame node");
    }

    ctx.mainFBO = mainFBO.get();
    for(auto& fbo : extraFBOs) {
        ctx.exraFBOs.push_back(fbo.get());
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
}

bool RenderGraph::startFrame() {
    prepareNodes();

    ET_SendEventReturn(ctx.proj2dMat, &ETRenderCamera::ET_getProj2DMat4);

    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETRenderCamera::ET_getRenderPort);

    bool canDraw = true;

    if(mainFBO->texture.getSize() != viewport) {
        mainFBO->texture.bind();
        if(!mainFBO->texture.resize(viewport)) {
            canDraw = false;
            LogError("[RenderGraph::startFrame] Can't resize framebuffer to size: %dx%d", viewport.x, viewport.y);
        }
        mainFBO->texture.unbind();
    }

    if(!canDraw) {
        return false;
    }

    RenderUtils::ClearFramebuffer(clearColor, *mainFBO);

    mainFBO->bind();

    return true;
}

void RenderGraph::endFrame() {
    ctx.setBlending(RenderBlendingType::NONE);
    mainFBO->unbind();
}

void RenderGraph::prepareNodes() {
    if(!needReorder) {
        return;
    }
    std::sort(children.begin(), children.end(), [](Node* first, Node* second){
        return first->getDrawPriority() < second->getDrawPriority();
    });
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

void RenderGraph::render() {
    if(!startFrame()) {
        return;
    }
    for(auto node : children) {
        node->render(ctx);
    }
    ET_SendEvent(&ETDebugRender::ET_update);
    endFrame();
    drawFrameNode.draw(*mainFBO);
}

void RenderGraph::renderToBuffer(ImageBuffer& imageBuffer, DrawContentFilter filter) {
    if(!startFrame()) {
        return;
    }
    for(auto node : children) {
        node->render(ctx);
    }
    if(filter != DrawContentFilter::NoDebugInfo) {
        ET_SendEvent(&ETDebugRender::ET_update);
    }
    endFrame();
    RenderUtils::ReadFramebufferToImage(*mainFBO, imageBuffer);
}