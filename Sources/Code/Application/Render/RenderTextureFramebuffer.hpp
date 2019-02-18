#ifndef __RENDER_TEXTURE_FRAMEBUFFER_HPP__
#define __RENDER_TEXTURE_FRAMEBUFFER_HPP__

#include "Color.hpp"
#include "Platforms/OpenGL.hpp"

#include <memory>

class RenderTextureFramebuffer {
public:

    RenderTextureFramebuffer(GLsizei w, GLsizei h);
    ~RenderTextureFramebuffer();

    bool init();
    bool bind();
    bool isBinded() const;
    void unbind();
    bool read();
    void clear();
    const void* getPtr() const;

    GLsizei getWidth() const;
    GLsizei getHeight() const;
    int getCompCount() const;
    ColorF getColor(GLsizei i, GLsizei j) const;

private:

    GLsizei width;
    GLsizei height;
    GLuint framebufferId;
    GLuint textureId;
    std::unique_ptr<Color::Color_RGBA_Byte[]> textureBuffer;
};

#endif /* __RENDER_TEXTURE_FRAMEBUFFER_HPP__ */