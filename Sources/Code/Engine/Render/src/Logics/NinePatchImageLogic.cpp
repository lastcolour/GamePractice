#include "Logics/NinePatchImageLogic.hpp"
#include "RenderTexture.hpp"
#include "RenderUtils.hpp"
#include "Math/MatrixTransform.hpp"

void NinePatchImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<NinePatchImageLogic>("NinePatchImageLogic")) {
        classInfo->addBaseClass<RenderImageLogic>();
        classInfo->addField("horizontal", &NinePatchImageLogic::horizontal);
        classInfo->addField("vertical", &NinePatchImageLogic::vertical);
    }
}

NinePatchImageLogic::NinePatchImageLogic() :
    RenderImageLogic(EDrawCmdType::NinePatch),
    horizontal(0.3f),
    vertical(0.3f) {
}

NinePatchImageLogic::~NinePatchImageLogic() {
}

void NinePatchImageLogic::onInit() {
    RenderImageLogic::onInit();

    horizontal = Math::Clamp(horizontal, 0.f, 0.4999f);
    vertical = Math::Clamp(vertical, 0.f, 0.4999f);

    auto ninePatchCmd = static_cast<DrawNinePatchCmd*>(cmd);
    ninePatchCmd->ninePatches = Vec2(horizontal, vertical);

    ETNode<ETNinePatchImageLogic>::connect(getEntityId());
}

Vec2 NinePatchImageLogic::ET_getPatches() const {
    return Vec2(horizontal, vertical);
}

void NinePatchImageLogic::ET_setPatches(const Vec2& patches) {
    horizontal = patches.x;
    horizontal = Math::Clamp(horizontal, 0.f, 0.4999f);
    vertical = patches.y;
    vertical = Math::Clamp(vertical, 0.f, 0.4999f);

    DrawNinePatchCmd::QueueNinePatchUpdate(*cmd, Vec2(horizontal, vertical));
}

Mat4 NinePatchImageLogic::calcModelMat() const {
    Mat4 mat = getTransform().toMat4();
    Vec3 scale(size.x * normScale / 2.f, size.y * normScale / 2.f, 1.f);
    Math::AddScale3D(mat, scale);
    return mat;
}

void NinePatchImageLogic::onTransformChanged(const Transform& newTm) {
    if(cmd) {
        const Mat4 modelMat = calcModelMat();
        DrawNinePatchCmd::QueueModelMatUpdate(*cmd, modelMat);
    }
}

void NinePatchImageLogic::ET_setSize(const Vec2& newSize) {
    Vec2 newSizeCopy = newSize;
    if(newSizeCopy.x <= 0.f || newSizeCopy.y < 0.f) {
        LogError("[NinePatchImageLogic::ET_setSize] Negative size: <%.1f, %.1f> (Entity: %s)", newSizeCopy.x, newSizeCopy.y,
            getEntityName());
        newSizeCopy.x = std::max(1.f, newSizeCopy.x);
        newSizeCopy.y = std::max(1.f, newSizeCopy.y);
    }

    DrawNinePatchCmd::QueueSizeUpdate(*cmd, size, newSizeCopy);
    size = newSizeCopy;
}

void NinePatchImageLogic::ET_setTextureInfo(const TextureInfo& newTexture) {
    textureInfo = newTexture;
    DrawNinePatchCmd::QueueTexInfoUpdate(*cmd, newTexture);
}

Vec2 NinePatchImageLogic::ET_getPatchesVertCoord() const {
    auto ninePatchCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
    if(!ninePatchCmd || !ninePatchCmd->texObj) {
        return Vec2(0.f);
    }
    const auto& tm = getTransform();
    return RenderUtils::GetNinePatchVertexCoord(ninePatchCmd->texObj->getSize(),
        Vec2(size.x * tm.scale.x, size.y * tm.scale.y), Vec2(horizontal, vertical));
}