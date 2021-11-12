#ifndef __RENDER_SHADER_HPP__
#define __RENDER_SHADER_HPP__

#include "Math/Matrix.hpp"
#include "Math/Color.hpp"

class RenderTexture;

enum class UniformType {
    CameraMat = 0,
    ModelMat,
    Alpha,
    Color,
    Texture,
    TextureSize,
    IsVerticalPass,
    ENUM_SIZE
};

class RenderShader {
public:

    explicit RenderShader(int progId);
    ~RenderShader();

    void bind();
    void unbind();

    void cacheUniformIds();

    void setTexture2d(UniformType varType, int unitId, RenderTexture& tex);
    void setUniform1i(UniformType varType, int val);
    void setUniform1f(UniformType varType, float val);
    void setUniform2f(UniformType varType, const Vec2i& val);
    void setUniform2f(UniformType varType, const Vec2& val);
    void setUniform4f(UniformType varType, const Vec4& vec);
    void setUniform4f(UniformType varType, const ColorB& col);
    void setUniformMat4(UniformType varType, const Mat4& mat);

    int getProgramId() const;
    void setProgramId(int newProgramId);

private:

    int getUniformId(UniformType varType) const;
    bool findUniform(const char* name, int& resUniLoc) const;

private:

    std::vector<int> chachedIds;
    int programId;
};

#endif /* __RENDER_SHADER_HPP__ */