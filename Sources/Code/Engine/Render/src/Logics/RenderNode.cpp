#include "Logics/RenderNode.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderMaterial.hpp"

RenderNode::RenderNode() :
    alpha(1.f),
    drawPriority(0),
    blending(RenderBlendingType::NONE),
    isVisible(true) {
}

RenderNode::~RenderNode() {
}

bool RenderNode::init() {
    ETNode<ETRenderNode>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
    ET_SendEvent(&ETRender::ET_updateRenderQueue);
    return true;
}

void RenderNode::deinit() {
}

void RenderNode::ET_setMaterial(const char* matName) {
    ET_SendEventReturn(mat, &ETRenderMaterialManager::ET_createMaterial, matName);
}

void RenderNode::ET_setGeometry(PrimitiveGeometryType geomType) {
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, geomType);
}

void RenderNode::ET_setAlpha(float newAlpha) {
    alpha = newAlpha;
}

bool RenderNode::ET_isVisible() const {
    if(!mat) {
        return false;
    }
    if(!geom) {
        return false;
    }
    return isVisible;
}

void RenderNode::ET_onRender(RenderContext& renderCtx) {
    if(!ET_isVisible()) {
        return;
    }

    if(blending != RenderBlendingType::NONE) {
        renderCtx.setBlending(blending);
    } else {
        if(alpha < 1.f) {
            renderCtx.setBlending(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
        } else {
            renderCtx.setBlending(blending);
        }
    }

    mat->bind();
    mat->setUniform1f("alpha", alpha);
    onRender(renderCtx);
    mat->unbind();
}

void RenderNode::ET_hide() {
    isVisible = false;
}

void RenderNode::ET_show() {
    isVisible = true;
}

void RenderNode::ET_setDrawPriority(int newDrawPriority) {
    drawPriority = newDrawPriority;
    ET_SendEvent(&ETRender::ET_updateRenderQueue);
}

int RenderNode::ET_getDrawPriority() const {
    return drawPriority;
}

void RenderNode::setBlendingMode(RenderBlendingType newBlending) {
    blending = newBlending;
}