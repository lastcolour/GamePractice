#ifndef __RENDER_COMMON_HPP__
#define __RENDER_COMMON_HPP__

enum class EStencilOpType {
    Disabled = 0,
    Read,
    ReadIncrease,
    Write
};

class StencilWirteReadData {
public:

    EStencilOpType mode;
    int refVal;

public:

    StencilWirteReadData() :
        mode(EStencilOpType::Disabled),
        refVal(0) {}
};

enum class PrimitiveGeometryType {
    Square = 0,
    Sqaure_Tex,
    Text,
    Particles,
    NinePatch,
    Line
};

enum class EFramebufferType {
    Color = 0,
    Color_Depth_Stencil
};

enum BlendType {
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

enum class EFontType {
    Game,
    Debug
};

#endif /* __RENDER_COMMON_HPP__ */