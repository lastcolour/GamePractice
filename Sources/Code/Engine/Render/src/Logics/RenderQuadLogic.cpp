#include "Logics/RenderQuadLogic.hpp"

RenderQuadLogic::RenderQuadLogic() :
    RenderNode(RenderNodeType::Quad),
    size(100.f) {
}

RenderQuadLogic::~RenderQuadLogic() {
}

void RenderQuadLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderQuadLogic>("RenderQuadLogic")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("shader", &RenderQuadLogic::shaderParams);
    }
}

void RenderQuadLogic::onInit() {
}