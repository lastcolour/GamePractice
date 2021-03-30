#include "Logics/RenderSimpleLogic.hpp"
#include "Nodes/SimpleNode.hpp"

RenderSimpleLogic::RenderSimpleLogic() :
    RenderNode(RenderNodeType::Simple),
    size(20.f),
    color(255, 255, 255) {
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

void RenderSimpleLogic::onInit() {
    auto simpleProxyNode = static_cast<SimpleNode*>(proxyNode);
    simpleProxyNode->setSize(size);
    simpleProxyNode->setColor(color);

    ETNode<ETRenderSimpleLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
}

void RenderSimpleLogic::ET_setColor(const ColorB& newColor) {
    color = newColor;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newColor](){
        auto simpleProxyNode = static_cast<SimpleNode*>(node);
        simpleProxyNode->setColor(newColor);
    });
}

void RenderSimpleLogic::ET_setSize(const Vec2& newSize) {
    size = newSize;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newSize](){
        auto simpleProxyNode = static_cast<SimpleNode*>(node);
        simpleProxyNode->setSize(newSize);
    });
}

Vec2 RenderSimpleLogic::ET_getSize() const {
    return size;
}