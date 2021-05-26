#ifndef __RENDER_SHADERS_PARAMS_HPP__
#define __RENDER_SHADERS_PARAMS_HPP__

#include "Render/TextureInfo.hpp"

enum class EShaderType {
    Solid = 0,
    Gradient,
    Image,
    Text,
    Particles
};

class RenderShaderParams {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderShaderParams(EShaderType shaderType);
    virtual ~RenderShaderParams() = 0;

public:

    EShaderType type;
};

class SolidColorShaderParams : public RenderShaderParams {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SolidColorShaderParams();

public:

    ColorB color;
};

class GradientColorShaderParams : public RenderShaderParams {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GradientColorShaderParams();

public:

    ColorB startCol;
    ColorB endCol;
    bool isVertical;
};

class ImageShaderParams : public RenderShaderParams {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ImageShaderParams();

public:

    TextureInfo texInfo;
};

class TextShaderParams : public RenderShaderParams {
public:

    static void Reflect(ReflectContext& ctx);

public:

    TextShaderParams();

public:

    ColorB color;
};

class ParticlesShaderParams : public RenderShaderParams {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesShaderParams();

public:

    TextureInfo texInfo;
};

void ReflectShadersParams();

#endif /* __RENDER_SHADERS_PARAMS_HPP__ */