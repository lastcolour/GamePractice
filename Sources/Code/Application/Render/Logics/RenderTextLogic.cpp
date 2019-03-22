#include "Render/Logics/RenderTextLogic.hpp"
#include "Render/RenderFont.hpp"

namespace {
    const char* TEXT_RENDER_MATERIAL = "text_solid_color";
} // namespace

RenderTextLogic::RenderTextLogic() {
}

RenderTextLogic::~RenderTextLogic() {
}

bool RenderTextLogic::serialize(const JSONNode& node) {
    return true;
}

bool RenderTextLogic::init() {
    ET_SendEventReturn(font, &ETRender::ET_createDefaultFont);
    if(!font) {
        return false;
    }
    ET_SendEventReturn(mat, &ETRender::ET_createMaterial, TEXT_RENDER_MATERIAL);
    if(!mat) {
        return false;
    }
    ETNode<ETRenderTextLogic>::connect(getEntityId());
    return true;
}

void RenderTextLogic::ET_onRender(const RenderContext& renderCtx) {
    for(auto ch : text) {
        if(auto glyph = font->getGlyph(ch)) {
        }
    }
}

void RenderTextLogic::ET_setText(const std::string& str) {
    text = str;
    if(!text.empty()) {
        ETNode<ETRenderEvents>::connect(getEntityId());
    } else {
        ETNode<ETRenderEvents>::disconnect();
    }
}
