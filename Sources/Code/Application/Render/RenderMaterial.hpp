#ifndef __RENDER_MATERIAL_HPP__
#define __RENDER_MATERIAL_HPP__

#include "Math/Matrix.hpp"
#include "Render/Color.hpp"

#include <string>

class RenderMaterial {
public:

    RenderMaterial(int progId);
    ~RenderMaterial();

    void bind();
    void unbind();

    void setUniform4f(const std::string& name, const Vec4& vec);
    void setUniform4f(const std::string& name, const ColorB& col);
    void setUniformMat4(const std::string& name, const Mat4& mat);

private:

    bool findUniform(const std::string& name, int& resUniLoc) const;

private:

    int programId;
};

#endif /* __RENDER_MATERIAL_HPP__ */