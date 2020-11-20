#include "Nodes/Node.hpp"
#include "RenderGraph/RenderGraph.hpp"
#include "Render/ETRenderManager.hpp"
#include "Core/ETPrimitives.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Platform/OpenGL.hpp"

#include <cassert>

Node::Node() :
    renderGraph(nullptr),
    alpha(1.f),
    drawPriority(0),
    blending(RenderBlendingType::NONE),
    visible(false) {
}

Node::~Node() {
}

void Node::preInit(const RenderNodeCreateParams& params) {
    type = params.type;
    alpha = params.alpha;
    drawPriority = params.drawPriority;
    tm = params.tm;
}

void Node::init() {
    onInit();
}

void Node::setAlpha(float newAlpha) {
    alpha = newAlpha;
}

void Node::setDrawPriority(int newDrawPriority) {
    drawPriority = newDrawPriority;
    if(renderGraph) {
        renderGraph->setNeedReorderNodes();
    }
}

void Node::setVisible(bool flag) {
    visible = flag;
}

void Node::setBlendingMode(RenderBlendingType newBlending) {
    blending = newBlending;
}

void Node::setShader(const char* shaderName) {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, shaderName);
}

void Node::setGeometry(PrimitiveGeometryType geomType) {
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, geomType);
}

void Node::setTransform(const Transform& newTm) {
    tm = newTm;
}

bool Node::isVisible() const {
    if(!shader) {
        return false;
    }
    if(!geom) {
        return false;
    }
    return visible;
}

void Node::setRenderGraph(RenderGraph* graph) {
    renderGraph = graph;
}

void Node::setStencilData(const StencilWirteReadData& newStencilData) {
    stencilData = newStencilData;
}

int Node::getDrawPriority() const {
    return drawPriority;
}
RenderNodeType Node::getType() const {
    return type;
}

void Node::onRenderStart(RenderContext& ctx) {
    if(alpha < 1.f && blending == RenderBlendingType::NONE) {
        ctx.setBlending(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    } else {
        ctx.setBlending(blending);
    }

    ctx.setStencilState(stencilData);
    shader->bind();
    shader->setUniformMat4(UniformType::CameraMat, ctx.proj2dMat);
    shader->setUniform1f(UniformType::Alpha, alpha);
}

void Node::onRenderEnd(RenderContext& ctx) {
    shader->unbind();
}

void Node::render(RenderContext& ctx) {
    onRenderStart(ctx);
    onRender(ctx);
    onRenderEnd(ctx);
}