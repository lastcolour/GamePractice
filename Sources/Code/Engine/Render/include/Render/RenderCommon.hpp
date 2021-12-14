#ifndef __RENDER_COMMON_HPP__
#define __RENDER_COMMON_HPP__

#include "Math/Color.hpp"

enum class EStencilOpType {
    Disabled = 0,
    Read,
    ReadIncrease,
    Write
};

enum class EPrimitiveGeometryType {
    Square = 0,
    Sqaure_Tex,
    Text,
    Particles,
    NinePatch,
    Line
};

enum class EBlendOp {
    NONE = 0,
    ZERO,
    ONE,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,
    CONSTANT_COLOR,
    ONE_MINUS_CONSTANT_COLOR,
    CONSTANT_ALPHA,
    ONE_MINUS_CONSTANT_ALPHA
};

enum class EBlendMode {
    None = 0,
    Normal,
    Additive,
    Multiply,
    Screen
};

enum class EFramebufferType {
    Color = 0,
    Color_Depth_Stencil
};

enum class EFontType {
    Game,
    Debug
};

enum class ETextureLerpType {
    Point = 0,
    Linear
};

enum class ETextureWrapType {
    Repeat = 0,
    MirroredRepeat,
    ClampToEdge
};

enum class ETextureDataType {
    R8 = 0,
    RA,
    RGB,
    RGBA
};

class StencilWirteReadData {
public:

    EStencilOpType opType{EStencilOpType::Disabled};
    int refVal{0};
};

struct BlendOpPair {
    EBlendOp src{EBlendOp::NONE};
    EBlendOp dst{EBlendOp::NONE};
};

class TextureInfo {
public:

    static void Reflect(ReflectContext& ctx);

public:

    TextureInfo() :
        lerpType(ETextureLerpType::Linear),
        tintColor(0, 0, 0, 0) {}

public:

    std::string filename;
    ETextureLerpType lerpType;
    ColorB tintColor;
};

#endif /* __RENDER_COMMON_HPP__ */