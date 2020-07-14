#include "Logics/RenderColoredTextureLogic.hpp"
#include "RenderMaterial.hpp"
#include "RenderGeometry.hpp"
#include "RenderTexture.hpp"
#include "RenderContext.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Reflect/ReflectContext.hpp"

RenderColoredTextureLogic::RenderColoredTextureLogic() :
    color(255, 255, 255) {

    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
}

RenderColoredTextureLogic::~RenderColoredTextureLogic() {
}

void RenderColoredTextureLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderColoredTextureLogic>("RenderColoredTexture")) {
        classInfo->addBaseClass<RenderImageLogic>();
        classInfo->addField("color", &RenderColoredTextureLogic::color);
    }
}

void RenderColoredTextureLogic::ET_setTextureColor(const ColorB& newColor) {
    color = newColor;
}

bool RenderColoredTextureLogic::init() {
    if(!RenderImageLogic::init()) {
        return false;
    }
    ET_setMaterial("tex_solid_color");
    if(!mat) {
        return false;
    }
    ETNode<ETRenderColoredTexture>::connect(getEntityId());
    return true;
}

void RenderColoredTextureLogic::onRender(RenderContext& renderCtx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 mvp = Render::CalcModelMat(getEntityId(), Vec3(scale, 1.f), *geom);
    mvp = renderCtx.proj2dMat * mvp;

    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    mat->setUniform4f("color", color);
    geom->draw();
}