#ifndef __TEXTURE_INFO_HPP__
#define __TEXTURE_INFO_HPP__

#include "Math/Color.hpp"

enum class ETextureLerpType {
    Point = 0,
    Linear
};

enum class ETextureWrapType {
    Repeat = 0,
    MirroredRepeat,
    ClamToEdge
};

enum class ETextureDataType {
    R8 = 0,
    RA,
    RGB,
    RGBA
};

class TextureInfo {
public:

    static void Reflect(ReflectContext& ctx);

public:

    TextureInfo();
    ~TextureInfo();

public:

    std::string filename;
    ETextureLerpType lerpType;
    ColorB tintColor;
};

#endif /* __TEXTURE_INFO_HPP__ */