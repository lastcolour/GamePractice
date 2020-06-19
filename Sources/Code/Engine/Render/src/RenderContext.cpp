#include "RenderContext.hpp"
#include "Platforms/OpenGL.hpp"

RenderContext::RenderContext() :
    dt(0.f),
    blendingType(RenderBlendingType::NONE) {
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
    case RenderBlendingType::SRC_MINUS_ALPHA:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    default:
        break;
    }
}