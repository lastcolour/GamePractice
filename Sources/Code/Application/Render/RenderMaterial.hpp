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

    void setTexture2D(const char* name, int texId);
    void setUniform1i(const char* name, int val);
    void setUniform4f(const char* name, const Vec4& vec);
    void setUniform4f(const char* name, const ColorB& col);
    void setUniformMat4(const char* name, const Mat4& mat);

    int getProgramId() const;
    void setProgramId(int newProgramId);

private:

    bool findUniform(const char* name, int& resUniLoc) const;

private:

    unsigned int activeTexUnitId;
    int programId;
};

#endif /* __RENDER_MATERIAL_HPP__ */