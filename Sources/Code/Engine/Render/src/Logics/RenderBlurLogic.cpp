#include "Logics/RenderBlurLogic.hpp"
#include "Nodes/BlurNode.hpp"

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
    downScaleFactor(2) {
}

RenderBlurLogic::~RenderBlurLogic() {
}

void RenderBlurLogic::onInit() {
    auto blurProxyNode = static_cast<BlurNode*>(proxyNode);

    passes = std::max(passes, 1);
    blurProxyNode->setPasses(passes);

    downScaleFactor = std::max(downScaleFactor, 1);
    blurProxyNode->setDownScale(downScaleFactor);
}