#include "Logics/NinePatchImageLogic.hpp"
#include "RenderTexture.hpp"
#include "RenderUtils.hpp"
#include "Math/MatrixTransform.hpp"
#include "Reflect/EnumInfo.hpp"

namespace {
    Vec2 ClampPatchesXY(const Vec2& v) {
        Vec2 newV = v;
        newV.x = Math::Clamp(v.x, 0.f, 0.4999f);
        newV.y = Math::Clamp(v.y, 0.f, 0.4999f);
        return newV;
    }
}

void NinePatchImageLogic::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<ENinePatchSizeType>("ENinePatchSizeType")) {
        enumInfo->addValues<ENinePatchSizeType>({
            {"SmallestSide", ENinePatchSizeType::SmallestSidePct},
            {"BiggestSide", ENinePatchSizeType::BiggestSidePct},
            {"Pixels", ENinePatchSizeType::Pixels}
        });
    }
    if(auto classInfo = ctx.classInfo<NinePatchImageLogic>("NinePatchImageLogic")) {
        classInfo->addBaseClass<RenderImageLogic>();
        classInfo->addField("patchesTextureCoords", &NinePatchImageLogic::patchesTextureCoords);
        classInfo->addField("patchSize", &NinePatchImageLogic::patchSize);
        classInfo->addField("patchSizeType", &NinePatchImageLogic::patchSizeType);
    }
}

NinePatchImageLogic::NinePatchImageLogic() :
    RenderImageLogic(EDrawCmdType::NinePatch),
    patchesTextureCoords(0.3f),
    patchSize(0.3f),
    patchSizeType(ENinePatchSizeType::SmallestSidePct) {
}

NinePatchImageLogic::~NinePatchImageLogic() {
}

void NinePatchImageLogic::onInit() {
    RenderImageLogic::onInit();

    patchesTextureCoords = ClampPatchesXY(patchesTextureCoords);

    auto ninePatchCmd = static_cast<DrawNinePatchCmd*>(cmd);
    ninePatchCmd->patchesTextureCoords = patchesTextureCoords;
    ninePatchCmd->patchSize = patchSize;
    ninePatchCmd->patchSizeType = patchSizeType;

    ETNode<ETNinePatchImageLogic>::connect(getEntityId());
}

Vec2 NinePatchImageLogic::ET_getPatchesTextureCoords() const {
    return patchesTextureCoords;
}

void NinePatchImageLogic::ET_setPatchesTextureCoords(const Vec2& newPatchesTextureCoords) {
    patchesTextureCoords = ClampPatchesXY(newPatchesTextureCoords);
    DrawNinePatchCmd::QueueNinePatchUpdate(*cmd, patchesTextureCoords, patchSize, patchSizeType);
}

Vec2 NinePatchImageLogic::ET_getPatchesVertCoord() const {
    auto ninePatchCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
    if(!ninePatchCmd || !ninePatchCmd->texObj) {
        return Vec2(0.f);
    }
    const auto& tm = getTransform();
    return RenderUtils::GetNinePatchVertexCoord(ninePatchCmd->texObj->getSize(),
        Vec2(size.x * tm.scale.x, size.y * tm.scale.y), patchSize, patchSizeType);
}

void NinePatchImageLogic::ET_setPatchSize(float newPatchSize) {
    patchSize = newPatchSize;
    DrawNinePatchCmd::QueueNinePatchUpdate(*cmd, patchesTextureCoords, patchSize, patchSizeType);
}

float NinePatchImageLogic::ET_getPatchSize() const {
    return patchSize;
}

void NinePatchImageLogic::ET_setPatchSizeType(ENinePatchSizeType newPatchSizeType) {
    if(newPatchSizeType != patchSizeType) {
        patchSizeType = newPatchSizeType;
        DrawNinePatchCmd::QueueNinePatchUpdate(*cmd, patchesTextureCoords, patchSize, patchSizeType);
    }
}

ENinePatchSizeType NinePatchImageLogic::ET_getPatchSizeType() const {
    return patchSizeType;
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
    if(newSizeCopy.x < 0.f || newSizeCopy.y < 0.f) {
        LogError("[NinePatchImageLogic::ET_setSize] Negative size: <%.1f, %.1f> (Entity: '%s')", newSizeCopy.x, newSizeCopy.y,
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