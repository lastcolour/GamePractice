#ifndef __RENDER_MATERIAL_HPP__
#define __RENDER_MATERIAL_HPP__

#include <string>

#include "Platforms/OpenGL.hpp"
#include "Render/Color.hpp"

class RenderMaterial {
public:

    RenderMaterial(GLuint progId);
    ~RenderMaterial();

    void bind();
    void unbind();

    void setUniform(const std::string& name, const ColorF& col);

public:

    GLuint programId;
};

#endif /* __RENDER_MATERIAL_HPP__ */