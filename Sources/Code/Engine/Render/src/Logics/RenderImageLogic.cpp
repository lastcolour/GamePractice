#include "Logics/RenderImageLogic.hpp"
#include "Math/MatrixTransform.hpp"

RenderImageLogic::RenderImageLogic() :
    RenderImageLogic(EDrawCmdType::TexturedQuad) {
}

RenderImageLogic::RenderImageLogic(EDrawCmdType cmdType) :
    DrawCommandProxy(cmdType),
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
            getEntityName());
    }

    if(size.x < 0.f || size.y < 0.f) {
        LogError("[RenderImageLogic::onInit] Negative size: <%.1f, %.1f> (Entity: %s)", size.x, size.y,
            getEntityName());
        size.x = std::max(1.f, size.x);
        size.y = std::max(1.f, size.y);
    }

    ETNode<ETRenderImageLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
}

Mat4 RenderImageLogic::calcModelMat() const {
    Mat4 mat = getTransform().toMat4();
    Vec3 scale(size.x * normScale / 2.f, size.y * normScale / 2.f, 1.f);
    Math::AddScale3D(mat, scale);
    return mat;
}

TextureInfo RenderImageLogic::ET_getTextureInfo() const {
    return textureInfo;
}

void RenderImageLogic::ET_setTextureInfo(const TextureInfo& newTextureInfo) {
    textureInfo = newTextureInfo;
    DrawTexturedQuadCmd::QueueTexInfoUpdate(*cmd, textureInfo);
}

void RenderImageLogic::ET_setSize(const Vec2& newSize) {
    Vec2 newSizeCopy = newSize;
    if(newSizeCopy.x < 0.f || newSizeCopy.y < 0.f) {
        LogError("[RenderImageLogic::ET_setSize] Negative size: <%.1f, %.1f> (Entity: %s)", newSizeCopy.x, newSizeCopy.y,
            getEntityName());
        newSizeCopy.x = std::max(1.f, newSizeCopy.x);
        newSizeCopy.y = std::max(1.f, newSizeCopy.y);
    }

    DrawColoredQuadCmd::QueueSizeUpdate(*cmd, size, newSizeCopy, cmdType);
    size = newSizeCopy;
}

Vec2 RenderImageLogic::ET_getSize() const {
    return size;
}