#include "Logics/RenderSimpleLogic.hpp"
#include "Math/MatrixTransform.hpp"

RenderSimpleLogic::RenderSimpleLogic() :
    DrawCommandProxy(EDrawCmdType::Quad),
    size(100.f),
    color(255, 255, 255) {
}

RenderSimpleLogic::~RenderSimpleLogic() {
}

void RenderSimpleLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderSimpleLogic>("RenderSimple")) {
        classInfo->addBaseClass<DrawCommandProxy>();
        classInfo->addField("size", &RenderSimpleLogic::size);
        classInfo->addField("color", &RenderSimpleLogic::color);
    }
}

void RenderSimpleLogic::onInit() {
    auto coloredQuadCmd = static_cast<DrawColoredQuadCmd*>(cmd);

    coloredQuadCmd->color = color;

    Math::AddScale(coloredQuadCmd->modelMat, Vec3(
        static_cast<float>(size.x), static_cast<float>(size.y), 1.f));

    ETNode<ETRenderSimpleLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
}

void RenderSimpleLogic::ET_setColor(const ColorB& newColor) {
    color = newColor;
    DrawColoredQuadCmd::QueueColorUpdate(*cmd, color);
}

void RenderSimpleLogic::ET_setSize(const Vec2& newSize) {
    DrawColoredQuadCmd::QueueSizeUpdate(*cmd, size, newSize, EDrawCmdType::Quad);
    size = newSize;
}

Vec2 RenderSimpleLogic::ET_getSize() const {
    return size;
}