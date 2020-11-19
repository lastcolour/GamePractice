#include "Logics/RenderBlurLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/BlurNode.hpp"

#include <algorithm>

void RenderBlurLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderBlurLogic>("RenderBlurLogic")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("downScaleFactor", &RenderBlurLogic::downScaleFactor);
        classInfo->addField("passes", &RenderBlurLogic::passes);
    }
}

RenderBlurLogic::RenderBlurLogic() :
    RenderNode(RenderNodeType::Blur),
    passes(1),
    downScaleFactor(2),
    isPassesChanged(true),
    isDownScaleChanged(true) {
}

RenderBlurLogic::~RenderBlurLogic() {
}

bool RenderBlurLogic::init() {
    isPassesChanged = true;
    isDownScaleChanged = true;
    return RenderNode::init();
}

void RenderBlurLogic::onSyncWithRender() {
    auto blurProxyNode = static_cast<BlurNode*>(proxyNode);
    if(isPassesChanged) {
        isPassesChanged = false;
        int val = std::max(downScaleFactor, 0);
        blurProxyNode->setPasses(val);
    }
    if(isDownScaleChanged) {
        isDownScaleChanged = false;
        int val = std::max(downScaleFactor, 1);
        blurProxyNode->setDownScale(val);
    }
}