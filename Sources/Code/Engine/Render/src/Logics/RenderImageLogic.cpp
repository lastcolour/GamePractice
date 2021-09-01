#include "Logics/RenderImageLogic.hpp"
#include "Nodes/ImageNode.hpp"

RenderImageLogic::RenderImageLogic() :
    RenderImageLogic(RenderNodeType::Image) {
}

RenderImageLogic::RenderImageLogic(RenderNodeType nodeType) :
    RenderNode(nodeType),
    size(100.f) {
}

RenderImageLogic::~RenderImageLogic() {
}

void RenderImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderImageLogic>("RenderImage")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("textureInfo", &RenderImageLogic::textureInfo);
        classInfo->addField("size", &RenderImageLogic::size);
    }
}

void RenderImageLogic::onInit() {
    auto imageProxyNode = static_cast<ImageNode*>(proxyNode);
    imageProxyNode->setSize(size);

    if(!textureInfo.filename.empty()) {
        ET_setTextureInfo(textureInfo);
    } else {
        LogWarning("[RenderImageLogic::onInit] Texture file is empty on entity: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
    }

    ETNode<ETRenderImageLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
}

TextureInfo RenderImageLogic::ET_getTextureInfo() const {
    return textureInfo;
}

void RenderImageLogic::ET_setTextureInfo(const TextureInfo& newTextureInfo) {
    textureInfo = newTextureInfo;
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode, texInfo=textureInfo](){
        auto imageProxyNode = static_cast<ImageNode*>(node);
        imageProxyNode->setTextureInfo(texInfo);
    });
}

void RenderImageLogic::ET_setSize(const Vec2& newSize) {
    size = newSize;
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode, newSize](){
        auto imageProxyNode = static_cast<ImageNode*>(node);
        imageProxyNode->setSize(newSize);
    });
}

Vec2 RenderImageLogic::ET_getSize() const {
    return size;
}