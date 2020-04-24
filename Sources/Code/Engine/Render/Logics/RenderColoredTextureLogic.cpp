#include "Render/Logics/RenderColoredTextureLogic.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/RenderTexture.hpp"
#include "Core/JSONNode.hpp"
#include "Render/RenderContext.hpp"
#include "Render/Logics/RenderAuxFunctions.hpp"

RenderColoredTextureLogic::RenderColoredTextureLogic() :
    color(255, 255, 255) {
}

RenderColoredTextureLogic::~RenderColoredTextureLogic() {
}

bool RenderColoredTextureLogic::serialize(const JSONNode& node) {
    if(!RenderImageLogic::serialize(node)) {
        return false;
    }
    Render::ReadColor(node.object("color"), color);
    if(node.hasKey("texture")) {
        std::string texName;
        node.read("texture", texName);
        ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, texName.c_str(), ETextureType::R8);
        if(!tex) {
            return false;
        }
    }
    return true;
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

void RenderColoredTextureLogic::ET_onRender(const RenderContext& renderCtx) {
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