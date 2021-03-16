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
    patchScale(1.f),
    isPatchChanged(true) {
}

NinePatchImageLogic::~NinePatchImageLogic() {
}

void NinePatchImageLogic::init() {
    RenderImageLogic::init();
    isPatchChanged = true;
    horizontal = Math::Clamp(horizontal, 0.f, 0.4999f);
    vertical = Math::Clamp(vertical, 0.f, 0.4999f);
    patchScale = Math::Clamp(patchScale, 0.f, 10.f);
}

void NinePatchImageLogic::onSyncWithRender() {
    RenderImageLogic::onSyncWithRender();
    auto ninePatchNode = static_cast<NinePatchNode*>(proxyNode);
    if(isPatchChanged) {
        isPatchChanged = false;
        ninePatchNode->setPatches(horizontal, vertical, patchScale);
    }
}