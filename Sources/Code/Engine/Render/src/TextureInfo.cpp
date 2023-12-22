#include "Render/RenderCommon.hpp"

void TextureInfo::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<TextureInfo>("TextureInfo")) {
        classInfo->addField("filename", &TextureInfo::filename);
        classInfo->addField("lerpType", &TextureInfo::lerpType);
        classInfo->addField("tintColor", &TextureInfo::tintColor);
    }
}

bool TextureInfo::operator==(const TextureInfo& other) const {
    return filename.get() == other.filename.get()
        && lerpType == other.lerpType
        && tintColor == other.tintColor
        && wrapType == other.wrapType;
}

bool TextureInfo::operator!=(const TextureInfo& other) const {
    return !(*this == other);
}