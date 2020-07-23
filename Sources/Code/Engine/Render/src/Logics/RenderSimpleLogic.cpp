#include "Logics/RenderSimpleLogic.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "RenderGeometry.hpp"
#include "RenderMaterial.hpp"
#include "RenderContext.hpp"
#include "Reflect/ReflectContext.hpp"

#include <cassert>

RenderSimpleLogic::RenderSimpleLogic() :
    size(20),
    color(255, 255, 255) {
}

RenderSimpleLogic::~RenderSimpleLogic() {
}

void RenderSimpleLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderSimpleLogic>("RenderSimple")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("size", &RenderSimpleLogic::size);
        classInfo->addField("color", &RenderSimpleLogic::color);
    }
}

bool RenderSimpleLogic::init() {
    ET_setGeometry(PrimitiveGeometryType::Square);
    ET_setMaterial("geom_solid_color");
    if(!RenderNode::init()) {
        return false;
    }
    ETNode<ETRenderSimpleLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
    return true;
}

void RenderSimpleLogic::onRender(RenderContext& renderCtx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 mvp = Render::CalcModelMat(getEntityId(), Vec3(scale, 1.f), *geom);
    mvp = renderCtx.proj2dMat * mvp;

    mat->setUniformMat4("MVP", mvp);
    mat->setUniform4f("color", color);
    geom->draw();
}

void RenderSimpleLogic::ET_setColor(const ColorB& col) {
    color = col;
}

void RenderSimpleLogic::ET_setSize(const Vec2i& newSize) {
    size = newSize;
}

Vec2i RenderSimpleLogic::ET_getSize() const {
    return size;
}