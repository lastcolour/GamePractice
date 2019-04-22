#ifndef __RENDER_MATERIAL_HPP__
#define __RENDER_MATERIAL_HPP__

#include "Math/Matrix.hpp"
#include "Render/Color.hpp"

#include <string>

class RenderMaterial {
public:

    explicit RenderMaterial(int progId);
    ~RenderMaterial();

    void bind();
    void unbind();

    void setTexture2D(const std::string& name, int texId);
    void setUniform1i(const std::string& name, int val);
    void setUniform4f(const std::string& name, const Vec4& vec);
    void setUniform4f(const std::string& name, const ColorB& col);
    void setUniformMat4(const std::string& name, const Mat4& mat);

private:

    bool findUniform(const std::string& name, int& resUniLoc) const;

private:

    unsigned int activeTexUnitId;
    int programId;
};

#endif /* __RENDER_MATERIAL_HPP__ */