#include "Nodes/Node.hpp"
#include "RenderGraph/RenderGraph.hpp"
#include "Render/ETRenderManager.hpp"
#include "Core/ETPrimitives.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Platform/OpenGL.hpp"

#include <cassert>

Node::Node() :
    renderGraph(nullptr),
    blendMode(BlendMode{BlendType::NONE, BlendType::NONE}),
    alpha(1.f),
    drawPriority(0),
    visible(false),
    modelMatDirty(true) {
}

Node::~Node() {
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

void Node::setBlendingMode(const BlendMode& newBlendMode) {
    blendMode = newBlendMode;
}

void Node::setShader(const char* shaderName) {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, shaderName);
}

void Node::setGeometry(PrimitiveGeometryType geomType) {
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, geomType);
}

void Node::setTransform(const Transform& newTm) {
    tm = newTm;
    setModelMatDirty();
}

bool Node::canRender() const {
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

void Node::onRenderStart(RenderContext& ctx) {
    if(alpha < 1.f && blendMode.dstBlending == BlendType::NONE) {
        ctx.setBlending(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    } else {
        ctx.setBlending(blendMode);
    }
    if(modelMatDirty) {
        modelMatDirty = false;
        modelMat = calcModelMat(tm);
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

void Node::setModelMatDirty() {
    modelMatDirty = true;
}