#include "Logics/RenderBlurLogic.hpp"

void RenderBlurLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderBlurLogic>("RenderBlurLogic")) {
        classInfo->addBaseClass<DrawCommandProxy>();
        classInfo->addField("downScaleFactor", &RenderBlurLogic::downScaleFactor);
        classInfo->addField("passes", &RenderBlurLogic::passes);
    }
}

RenderBlurLogic::RenderBlurLogic() :
    DrawCommandProxy(EDrawCmdType::Blur),
    passes(1),
    downScaleFactor(2) {
}

RenderBlurLogic::~RenderBlurLogic() {
}

void RenderBlurLogic::onInit() {
    auto blurCmd = static_cast<DrawBlurCmd*>(cmd);

    passes = std::max(passes, 1);
    blurCmd->passes = passes;

    downScaleFactor = std::max(downScaleFactor, 1);
    blurCmd->downScale = downScaleFactor;
}

Mat4 RenderBlurLogic::calcModelMat() const {
    return getTransform().toMat4();
}