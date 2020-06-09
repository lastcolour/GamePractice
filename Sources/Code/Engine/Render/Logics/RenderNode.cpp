#include "Render/Logics/RenderNode.hpp"
#include "Core/JSONNode.hpp"

RenderNode::RenderNode() :
    drawPriority(0),
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
    onRender(renderCtx);
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