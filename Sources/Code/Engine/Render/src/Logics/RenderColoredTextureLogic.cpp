#include "Logics/RenderColoredTextureLogic.hpp"
#include "RenderMaterial.hpp"
#include "RenderGeometry.hpp"
#include "RenderTexture.hpp"
#include "RenderContext.hpp"
#include "Logics/RenderAuxFunctions.hpp"

RenderColoredTextureLogic::RenderColoredTextureLogic() :
    color(255, 255, 255) {
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
    ETNode<ETRenderColoredTexture>::connect(getEntityId());
    return true;
}

void RenderColoredTextureLogic::onRender(RenderContext& renderCtx) {
    Vec3 scale = Vec3(imageScale.x * texScale.x, imageScale.y * texScale.y, 1.f);
    Mat4 mvp = Render::CalcModelMat(getEntityId(), scale, *geom);
    mvp = renderCtx.proj2dMat * mvp;

    mat->bind();
    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    mat->setUniform4f("color", color);
    geom->draw();
    mat->unbind();
}