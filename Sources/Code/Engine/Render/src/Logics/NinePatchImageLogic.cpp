#include "Logics/NinePatchImageLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/NinePatchNode.hpp"

void NinePatchImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<NinePatchImageLogic>("NinePatchImageLogic")) {
        classInfo->addBaseClass<RenderImageLogic>();
        classInfo->addField("left", &NinePatchImageLogic::left);
        classInfo->addField("right", &NinePatchImageLogic::right);
        classInfo->addField("top", &NinePatchImageLogic::top);
        classInfo->addField("down", &NinePatchImageLogic::down);
    }
}

NinePatchImageLogic::NinePatchImageLogic() :
    RenderImageLogic(RenderNodeType::NinePatchImage),
    left(0.3f),
    right(0.3f),
    top(0.3f),
    down(0.3f),
    isPatchChanged(true) {
}

NinePatchImageLogic::~NinePatchImageLogic() {
}

void NinePatchImageLogic::onSyncWithRender() {
    RenderImageLogic::onSyncWithRender();
    auto ninePatchNode = static_cast<NinePatchNode*>(proxyNode);
    if(isPatchChanged) {
        isPatchChanged = false;
        ninePatchNode->setPatches(left, right, top, down);
    }
}