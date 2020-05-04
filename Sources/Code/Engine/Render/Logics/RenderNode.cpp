#include "Render/Logics/RenderNode.hpp"
#include "Core/JSONNode.hpp"

RenderNode::RenderNode() :
    isVisible(true),
    drawPriority(0) {
}

RenderNode::~RenderNode() {
}

void RenderNode::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderNode>("RenderNode")) {
        classInfo->addResourceField("geometry", &RenderNode::ET_setGeometry);
        classInfo->addResourceField("material", &RenderNode::ET_setMaterial);
    }
}

bool RenderNode::init() {
    ET_SendEvent(&ETRender::ET_updateRenderQueue);
    ETNode<ETRenderNode>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
    return true;
}

void RenderNode::deinit() {
}

void RenderNode::ET_setMaterial(const char* matName) {
    ET_SendEventReturn(mat, &ETRenderMaterialManager::ET_createMaterial, matName);
}

void RenderNode::ET_setGeometry(const char* geomName) {
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, geomName);
}

bool RenderNode::ET_isVisible() const {
    return isVisible;
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

bool RenderNode::ET_getScrMinusAlphaBlendFlag() const {
    return false;
}
