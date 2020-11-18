#include "RenderGraph/DrawFrameNode.hpp"
#include "Render/ETRenderManager.hpp"
#include "Core/ETPrimitives.hpp"
#include "RenderShader.hpp"
#include "RenderGeometry.hpp"
#include "RenderGeometry.hpp"
#include "RenderFramebuffer.hpp"
#include "RenderTexture.hpp"

DrawFrameNode::DrawFrameNode() {
}

DrawFrameNode::~DrawFrameNode() {
}

bool DrawFrameNode::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "draw_framebuffer");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, PrimitiveGeometryType::Sqaure_Tex);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawFrameNode::draw(RenderFramebuffer& framebuffer) {
    shader->bind();
    shader->setTexture2D(UniformType::Texture, framebuffer.texture.texId);
    geom->draw();
    shader->unbind();
}