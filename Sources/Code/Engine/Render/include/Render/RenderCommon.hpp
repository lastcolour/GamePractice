#ifndef __RENDER_COMMON_HPP__
#define __RENDER_COMMON_HPP__

#include "Math/Color.hpp"
#include "Reflect/Resource.hpp"

enum class EStencilOpType {
    Disabled = 0,
    Read,
    ReadIncrease,
    Write
};

enum class EPrimitiveGeometryType {
    Vec2_Tex = 0,
    Vec3,
    Vec3_Tex,
    Particles,
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
    Unset = 0,
    Point,
    Linear
};

enum class ETextureWrapType {
    Unset = 0,
    Repeat,
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

    struct ResourceDescriptor {
        using RuntimeType = std::string;
        using StorageType = std::string;
        const char* TypeName = "Image";
    };

public:

    TextureInfo() :
        wrapType(ETextureWrapType::ClampToEdge),
        lerpType(ETextureLerpType::Linear),
        tintColor(0, 0, 0, 0) {}

    bool operator==(const TextureInfo& other) const;
    bool operator!=(const TextureInfo& other) const;

public:

    Reflect::Resource<ResourceDescriptor> filename;
    ETextureWrapType wrapType;
    ETextureLerpType lerpType;
    ColorB tintColor;
};

#endif /* __RENDER_COMMON_HPP__ */