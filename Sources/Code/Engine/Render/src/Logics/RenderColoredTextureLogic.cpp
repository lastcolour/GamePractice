#include "Logics/RenderColoredTextureLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ColoredTextureNode.hpp"

RenderColoredTextureLogic::RenderColoredTextureLogic() :
    RenderImageLogic(RenderNodeType::ColoredTexture),
    color(255, 255, 255),
    isColorChanged(true) {
}

RenderColoredTextureLogic::~RenderColoredTextureLogic() {
}

void RenderColoredTextureLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderColoredTextureLogic>("RenderColoredTexture")) {
        classInfo->addBaseClass<RenderImageLogic>();
        classInfo->addField("color", &RenderColoredTextureLogic::color);
    }
}

void RenderColoredTextureLogic::ET_setTextureColor(const ColorB& newColor) {
    color = newColor;
    isColorChanged = true;
    markForSyncWithRender();
}

bool RenderColoredTextureLogic::init() {
    RenderImageLogic::init();
    ETNode<ETRenderColoredTexture>::connect(getEntityId());
    ET_setImage(image.c_str());
    ET_setTextureColor(color);
    ET_setSize(size);
    return true;
}

void RenderColoredTextureLogic::onSyncWithRender() {
    RenderImageLogic::onSyncWithRender();
    auto coloredTextureProxyNode = static_cast<ColoredTextureNode*>(proxyNode);
    if(isColorChanged) {
        isColorChanged = false;
        coloredTextureProxyNode->setColor0(color);
    }
}