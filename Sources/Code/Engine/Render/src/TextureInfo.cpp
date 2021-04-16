#include "Render/TextureInfo.hpp"
#include "Reflect/EnumInfo.hpp"

void TextureInfo::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<ETextureLerpType>("ETextureLerpType")) {
        enumInfo->addValues<ETextureLerpType>({
            {"Point", ETextureLerpType::Point},
            {"Linear", ETextureLerpType::Linear}
        });
    }
    if(auto classInfo = ctx.classInfo<TextureInfo>("TextureInfo")) {
        classInfo->addResourceField("filename", ResourceType::Image, &TextureInfo::filename);
        classInfo->addField("lerpType", &TextureInfo::lerpType);
        classInfo->addField("tintColor", &TextureInfo::tintColor);
    }
}

TextureInfo::TextureInfo() :
    lerpType(ETextureLerpType::Point),
    tintColor(0, 0, 0, 0) {
}

TextureInfo::~TextureInfo() {
}