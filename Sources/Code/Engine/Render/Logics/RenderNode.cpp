#include "Render/Logics/RenderNode.hpp"
#include "Core/JSONNode.hpp"

RenderNode::RenderNode() :
    isVisible(true),
    drawPriority(0) {
}

RenderNode::~RenderNode() {
}

bool RenderNode::serialize(const JSONNode& node) {
    if(node.hasKey("geom")) {
        std::string geomName;
        node.read("geom", geomName);
        ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, geomName.c_str());
        if(!geom) {
            return false;
        }
    }
    if(node.hasKey("mat")) {
        std::string matName;
        node.read("mat", matName);
        ET_SendEventReturn(mat, &ETRenderMaterialManager::ET_createMaterial, matName.c_str());
        if(!mat) {
            return false;
        }
    }
    return true;
}

bool RenderNode::init() {
    ET_SendEvent(&ETRender::ET_updateRenderQueue);
    ETNode<ETRenderNode>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
    return true;
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
