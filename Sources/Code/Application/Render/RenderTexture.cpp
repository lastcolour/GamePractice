#include "Render/RenderTexture.hpp"
#include "Platforms/OpenGL.hpp"

RenderTexture::RenderTexture() :
    texId(0),
    size(0) {
}

RenderTexture::~RenderTexture() {
    glDeleteTextures(1, &texId);
}

void RenderTexture::bind() {
}

void RenderTexture::undind() {
}