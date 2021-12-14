#include "Logics/NinePatchImageLogic.hpp"
#include "RenderUtils.hpp"

void NinePatchImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<NinePatchImageLogic>("NinePatchImageLogic")) {
        classInfo->addBaseClass<RenderImageLogic>();
        classInfo->addField("horizontal", &NinePatchImageLogic::horizontal);
        classInfo->addField("vertical", &NinePatchImageLogic::vertical);
        classInfo->addField("patchScale", &NinePatchImageLogic::patchScale);
    }
}

NinePatchImageLogic::NinePatchImageLogic() :
    horizontal(0.3f),
    vertical(0.3f),
    patchScale(1.f) {
}

NinePatchImageLogic::~NinePatchImageLogic() {
}

void NinePatchImageLogic::onInit() {
    RenderImageLogic::onInit();

    horizontal = Math::Clamp(horizontal, 0.f, 0.4999f);
    vertical = Math::Clamp(vertical, 0.f, 0.4999f);
    patchScale = Math::Clamp(patchScale, 0.f, 10.f);

    auto ninePatchCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
    ninePatchCmd->imageType = DrawTexturedQuadCmd::EImageCmdType::NinePatch;
    ninePatchCmd->ninePatches = Vec2(horizontal, vertical);
    ninePatchCmd->patchesScale = patchScale;

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

    DrawTexturedQuadCmd::QueueNinePatchUpdate(*cmd, Vec2(horizontal, vertical));
}

Vec2 NinePatchImageLogic::ET_getPatchesVertCoord() const {
    auto ninePatchCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
    if(!ninePatchCmd || !ninePatchCmd->texObj) {
        return Vec2(0.f);
    }
    return Vec2(0.f);
    // return RenderUtils::GetNinePatchVertexCoord(ninePatchCmd->texObj->getSize(),
    //     Vec2(size.x * tm.scale.x, size.y * tm.scale.y), Vec2(horizontal, vertical), patchScale);
}