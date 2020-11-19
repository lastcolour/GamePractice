#include "RenderContext.hpp"
#include "Platform/OpenGL.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Core/ETPrimitives.hpp"

RenderContext::RenderContext() :
    dt(0.f),
    blendingType(RenderBlendingType::NONE),
    mainFBO(nullptr) {
}

RenderContext::~RenderContext() {
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