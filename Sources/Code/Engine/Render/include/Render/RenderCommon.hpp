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

enum class ETextureType {
    R8,
    RGB,
    RGBA
};

enum class PrimitiveGeometryType {
    Square = 0,
    Sqaure_Tex,
    Text_Vert_Chunk,
    Particles
};

enum class EFramebufferType {
    Color = 0,
    Color_Depth_Stencil
};

#endif /* __RENDER_COMMON_HPP__ */