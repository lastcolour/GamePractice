#include "Logics/NinePatchImageLogic.hpp"
#include "Nodes/NinePatchNode.hpp"

void NinePatchImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<NinePatchImageLogic>("NinePatchImageLogic")) {
        classInfo->addBaseClass<RenderImageLogic>();
        classInfo->addField("horizontal", &NinePatchImageLogic::horizontal);
        classInfo->addField("vertical", &NinePatchImageLogic::vertical);
        classInfo->addField("patchScale", &NinePatchImageLogic::patchScale);
    }
}

NinePatchImageLogic::NinePatchImageLogic() :
    RenderImageLogic(RenderNodeType::NinePatchImage),
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

    auto ninePatchNode = static_cast<NinePatchNode*>(proxyNode);
    ninePatchNode->setPatches(horizontal, vertical, patchScale);

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
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode, h=horizontal, v=vertical, s=patchScale](){
        auto ninePatchNode = static_cast<NinePatchNode*>(node);
        ninePatchNode->setPatches(h, v, s);
    });
}

Vec2 NinePatchImageLogic::ET_getPatchesVertCoord() const {
    if(!proxyNode) {
        return Vec2(0.f);
    }
    auto ninePatchNode = static_cast<NinePatchNode*>(proxyNode);
    return ninePatchNode->getVertCoord() / 2.f;
}