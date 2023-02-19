#include "Logics/RenderLinearGradientRect.hpp"
#include "Math/MatrixTransform.hpp"

RenderLinearGradientRect::RenderLinearGradientRect() :
    DrawCommandProxy(EDrawCmdType::TexturedQuad),
    size(100),
    startCol(255, 255, 255),
    endCol(0, 0, 0),
    isVertical(true) {
}

RenderLinearGradientRect::~RenderLinearGradientRect() {
}

void RenderLinearGradientRect::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderLinearGradientRect>("RenderLinearGradientRect")) {
        classInfo->addBaseClass<DrawCommandProxy>();
        classInfo->addField("size", &RenderLinearGradientRect::size);
        classInfo->addField("startColor", &RenderLinearGradientRect::startCol);
        classInfo->addField("endColor", &RenderLinearGradientRect::endCol);
        classInfo->addField("isVertical", &RenderLinearGradientRect::isVertical);
    }
}

void RenderLinearGradientRect::onInit() {
    auto texQuadCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
    texQuadCmd->imageType = DrawTexturedQuadCmd::EImageCmdType::Gradient;

    if(size.x <= 0.f || size.y < 0.f) {
        LogError("[RenderLinearGradientRect::onInit] Negative size: <%.1f, %.1f> (Entity: %s)", size.x, size.y,
            getEntityName());
        size.x = std::max(1.f, size.x);
        size.y = std::max(1.f, size.y);
    }

    DrawTexturedQuadCmd::QueueSetTexGradient(*cmd, startCol, endCol, isVertical);

    ETNode<ETRenderRect>::connect(getEntityId());
}

Mat4 RenderLinearGradientRect::calcModelMat() const {
    Mat4 mat = getTransform().toMat4();
    Vec3 scale(size.x * normScale / 2.f, size.y * normScale / 2.f, 1.f);
    Math::AddScale3D(mat, scale);
    return mat;
}

void RenderLinearGradientRect::ET_setSize(const Vec2& newSize) {
    Vec2 newSizeCopy = newSize;
    if(newSizeCopy.x <= 0.f || newSizeCopy.y < 0.f) {
        LogError("[RenderLinearGradientRect::ET_setSize] Negative size: <%.1f, %.1f> (Entity: %s)", newSizeCopy.x, newSizeCopy.y,
            getEntityName());
        newSizeCopy.x = std::max(1.f, newSizeCopy.x);
        newSizeCopy.y = std::max(1.f, newSizeCopy.y);
    }

    DrawColoredQuadCmd::QueueSizeUpdate(*cmd, size, newSizeCopy, cmdType);
    size = newSizeCopy;
}

Vec2 RenderLinearGradientRect::ET_getSize() const {
    return size;
}