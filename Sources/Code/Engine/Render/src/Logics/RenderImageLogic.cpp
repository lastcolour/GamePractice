#include "Logics/RenderImageLogic.hpp"

RenderImageLogic::RenderImageLogic() :
    DrawCommandProxy(EDrawCmdType::TexturedQuad),
    size(100.f) {
}

RenderImageLogic::~RenderImageLogic() {
}

void RenderImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderImageLogic>("RenderImage")) {
        classInfo->addBaseClass<DrawCommandProxy>();
        classInfo->addField("textureInfo", &RenderImageLogic::textureInfo);
        classInfo->addField("size", &RenderImageLogic::size);
    }
}

void RenderImageLogic::onInit() {
    auto texQuadCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
    texQuadCmd->imageType = DrawTexturedQuadCmd::EImageCmdType::Image;

    if(!textureInfo.filename.empty()) {
        texQuadCmd->texInfo = textureInfo;
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
    DrawTexturedQuadCmd::QueueTexInfoUpdate(*cmd, textureInfo);
}

void RenderImageLogic::ET_setSize(const Vec2& newSize) {
    DrawCmd::QueueSizeUpdate(*cmd, size, newSize, EDrawCmdType::TexturedQuad);
    size = newSize;
}

Vec2 RenderImageLogic::ET_getSize() const {
    return size;
}