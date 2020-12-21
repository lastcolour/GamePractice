#include "Logics/RenderSimpleLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/SimpleNode.hpp"

RenderSimpleLogic::RenderSimpleLogic() :
    RenderNode(RenderNodeType::Simple),
    size(20.f),
    color(255, 255, 255),
    isSizeChanged(true),
    isColorChanged(true) {
}

RenderSimpleLogic::~RenderSimpleLogic() {
}

void RenderSimpleLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderSimpleLogic>("RenderSimple")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("size", &RenderSimpleLogic::size);
        classInfo->addField("color", &RenderSimpleLogic::color);
    }
}

void RenderSimpleLogic::init() {
    RenderNode::init();
    ETNode<ETRenderSimpleLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
    ET_setColor(color);
    ET_setSize(size);
}

void RenderSimpleLogic::ET_setColor(const ColorB& col) {
    color = col;
    isColorChanged = true;
    markForSyncWithRender();
}

void RenderSimpleLogic::ET_setSize(const Vec2& newSize) {
    size = newSize;
    isSizeChanged = true;
    markForSyncWithRender();
}

Vec2 RenderSimpleLogic::ET_getSize() const {
    return size;
}

void RenderSimpleLogic::onSyncWithRender() {
    auto simpleProxyNode = static_cast<SimpleNode*>(proxyNode);
    if(isSizeChanged) {
        isSizeChanged = false;
        simpleProxyNode->setSize(size);
    }
    if(isColorChanged) {
        isColorChanged = false;
        simpleProxyNode->setColor0(color);
    }
}