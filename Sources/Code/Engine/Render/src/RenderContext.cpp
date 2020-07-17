#include "RenderContext.hpp"
#include "Platform/OpenGL.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Core/ETPrimitives.hpp"

RenderContext::RenderContext() :
    dt(0.f),
    clearColor(0, 0, 0),
    blendingType(RenderBlendingType::NONE) {
}

RenderContext::~RenderContext() {
}

void RenderContext::startFrame() {
    ET_SendEventReturn(proj2dMat, &ETRenderCamera::ET_getProj2DMat4);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    auto clearColF = clearColor.getColorF();
    glClearColor(clearColF.r, clearColF.g, clearColF.b, clearColF.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderContext::endFrame() {
    setBlending(RenderBlendingType::NONE);
}

void RenderContext::setBlending(RenderBlendingType newBlendingType) {
    if(newBlendingType == blendingType) {
        return;
    }
    blendingType = newBlendingType;
    switch(blendingType)
    {
    case RenderBlendingType::NONE:
        glDisable(GL_BLEND);
        break;
    case RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    default:
        break;
    }
}

RenderBlendingType RenderContext::getBlendingType() const {
    return blendingType;
}

void RenderContext::setClearColor(const ColorB& newColor) {
    clearColor = newColor;
}

const ColorB& RenderContext::getClearColor() const {
    return clearColor;
}