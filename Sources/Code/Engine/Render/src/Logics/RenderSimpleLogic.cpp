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

    if(size.x <= 0.f || size.y < 0.f) {
        LogError("[RenderSimpleLogic::onInit] Negative size: <%.1f, %.1f> (Entity: %s)", size.x, size.y,
            getEntityName());
        size.x = std::max(1.f, size.x);
        size.y = std::max(1.f, size.y);
    }

    Math::AddScale3D(coloredQuadCmd->modelMat, Vec3(
        static_cast<float>(size.x), static_cast<float>(size.y), 1.f));

    ETNode<ETRenderSimpleLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
}

Mat4 RenderSimpleLogic::calcModelMat() const {
    Mat4 mat = getTransform().toMat4();
    Vec3 scale(size.x * normScale / 2.f, size.y * normScale / 2.f, 1.f);
    Math::AddScale3D(mat, scale);
    return mat;
}

void RenderSimpleLogic::ET_setColor(const ColorB& newColor) {
    color = newColor;
    DrawColoredQuadCmd::QueueColorUpdate(*cmd, color);
}

void RenderSimpleLogic::ET_setSize(const Vec2& newSize) {
    Vec2 newSizeCopy = newSize;
    if(newSizeCopy.x <= 0.f || newSizeCopy.y < 0.f) {
        LogError("[RenderSimpleLogic::ET_setSize] Negative size: <%.1f, %.1f> (Entity: %s)", newSizeCopy.x, newSizeCopy.y,
            getEntityName());
        newSizeCopy.x = std::max(1.f, newSizeCopy.x);
        newSizeCopy.y = std::max(1.f, newSizeCopy.y);
    }

    DrawColoredQuadCmd::QueueSizeUpdate(*cmd, size, newSizeCopy, cmdType);
    size = newSizeCopy;
}

Vec2 RenderSimpleLogic::ET_getSize() const {
    return size;
}