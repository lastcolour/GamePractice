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
    ET_SendEventReturn(framebuffer, &ETRenderTextureManger::ET_createFramebuffer);
    if(!framebuffer) {
        LogError("[RenderGraph::init] Can't create framebuffer");
        assert(false && "Can't create framebuffer");
    }
    if(!drawFrameNode.init()) {
        LogError("[RenderGraph::init] Can't init draw frame node");
        assert(false && "Can't init draw frame node");
    }
}

void RenderGraph::startFrame() {
    prepareNodes();

    ET_SendEventReturn(ctx.proj2dMat, &ETRenderCamera::ET_getProj2DMat4);

    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETRenderCamera::ET_getRenderPort);

    if(framebuffer->texture.getSize() != viewport) {
        framebuffer->texture.bind();
        framebuffer->texture.resize(viewport);
        framebuffer->texture.unbind();
    }
    framebuffer->bind();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderGraph::endFrame() {
    ctx.setBlending(RenderBlendingType::NONE);
    framebuffer->unbind();
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
    startFrame();
    for(auto node : children) {
        node->render(ctx);
    }
    ET_SendEvent(&ETDebugRender::ET_update);
    endFrame();
    drawFrameNode.draw(*framebuffer);
}

void RenderGraph::renderToBuffer(ImageBuffer& imageBuffer, DrawContentFilter filter) {
    startFrame();
    for(auto node : children) {
        node->render(ctx);
    }
    if(filter != DrawContentFilter::NoDebugInfo) {
        ET_SendEvent(&ETDebugRender::ET_update);
    }
    endFrame();
    RenderUtils::ReadFramebufferToImage(*framebuffer, imageBuffer);
}