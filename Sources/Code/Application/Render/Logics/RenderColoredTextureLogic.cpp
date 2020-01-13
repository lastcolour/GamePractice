#include "Render/Logics/RenderColoredTextureLogic.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/RenderTexture.hpp"
#include "Core/JSONNode.hpp"
#include "Render/RenderContext.hpp"

RenderColoredTextureLogic::RenderColoredTextureLogic() :
    color(255, 255, 255) {
}

RenderColoredTextureLogic::~RenderColoredTextureLogic() {
}

bool RenderColoredTextureLogic::serialize(const JSONNode& node) {
    RenderImageLogic::serialize(node);
    if(auto colorNode = node.object("color")) {
        int val = 0;
        colorNode.read("r", val);
        color.r = val;
        colorNode.read("g", val);
        color.g = val;
        colorNode.read("b", val);
        color.b = val;
        colorNode.read("a", val);
        color.a = val;
    }
    std::string texName;
    node.read("texture", texName);
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, texName.c_str(), ETextureType::SingleColor);
    return true;
}

void RenderColoredTextureLogic::ET_setImage(const char* imageName) {
    if(!imageName || !imageName[0]) {
        tex.reset();
    } else {
        ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, imageName, ETextureType::SingleColor);
    }
    if(!tex) {
        ET_hide();
    } else {
        updateScale();
        ET_show();
    }
}

void RenderColoredTextureLogic::ET_setTextureColor(const ColorB& newColor) {
    color = newColor;
}

bool RenderColoredTextureLogic::init() {
    RenderImageLogic::init();
    ETNode<ETRenderColoredTexture>::connect(getEntityId());
    return true;
}

void RenderColoredTextureLogic::ET_onRender(const RenderContext& renderCtx) {
    renderCtx.setSrcMinusAlphaBlending(true);

    Mat4 mvp = getModelMat();
    mvp = renderCtx.proj2dMat * mvp;

    mat->bind();
    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    mat->setUniform4f("color", color);
    geom->draw();
    mat->unbind();
}