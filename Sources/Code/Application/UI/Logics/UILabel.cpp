#include "UI/Logics/UILabel.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

namespace {

const char* DEFAULT_TEXT_RENDERER = "Render/RenderTextSimple.json";

} // namespace

UILabel::UILabel() {
}

UILabel::~UILabel() {
}

bool UILabel::serialize(const JSONNode& node) {
    if(!UIBaseBox::serialize(node)) {
        LogWarning("[UILabel::serialize] Can't serialize UIbox");
        return false;
    }
    node.value("text", text);
    return true;
}

bool UILabel::init() {
    createRenderer();
    if(!UIBaseBox::init()) {
        LogWarning("[UILabel::init] Can't init UIbox");
        return false;
    }

    updateRendererParams();
    ETNode<ETUILabel>::connect(getEntityId());
    return true;
}

void UILabel::ET_setStyle(const UIStyle& newStyle) {
    bool isNewRenderer = false;
    if(ET_getStyle().renderer != newStyle.renderer) {
        createRenderer();
    }
    UIBaseBox::ET_setStyle(newStyle);
    updateRendererParams();
    onResizeFromTop();
}

void UILabel::ET_setText(const char* newText) {
    text = newText;
    ET_SendEvent(renderId, &ETRenderTextLogic::ET_setText, text.c_str());
    onResizeFromTop();
}

const char* UILabel::ET_getText() const {
    return text.c_str();
}

void UILabel::ET_onTransformChanged(const Transform& newTm) {
    UIBaseBox::ET_onTransformChanged(newTm);
}

void UILabel::createRenderer() {
    if(renderId.isValid()) {
        ET_SendEvent(&ETGameObjectManager::ET_destroyObject, renderId);
        renderId = InvalidEntityId;
    }
    const auto& boxStyle = ET_getStyle();
    std::string rendererName = boxStyle.renderer;
    if(rendererName.empty()) {
        rendererName = DEFAULT_TEXT_RENDERER;
    }
    ET_SendEventReturn(renderId, &ETGameObjectManager::ET_createGameObject, rendererName.c_str());
    if(renderId.isValid()) {
        ET_SendEvent(renderId, &ETGameObject::ET_setParent, getEntityId());
        ET_SendEvent(renderId, &ETRenderTextLogic::ET_setText, text.c_str());

        Transform tm;
        ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);
        ET_SendEvent(renderId, &ETGameObject::ET_setTransform, tm);
    } else {
        LogWarning("[UILabel::createRenderers] Can't create renderer: %s", rendererName);
    }
}

void UILabel::updateRendererParams() const {
    if(renderId.isValid()) {
        ET_SendEvent(renderId, &ETRenderTextLogic::ET_setColor, ET_getStyle().fontColor);
    }
}

Vec2i UILabel::calculateBoxSize(const AABB2Di& parentBox) const {
    auto parentBoxSize = parentBox.getSize();
    int renderFontSize = static_cast<int>(parentBoxSize.y * ET_getStyle().fontSize);
    ET_SendEvent(renderId, &ETRenderTextLogic::ET_setFontSize, renderFontSize);

    AABB2D renderTextBox(0.f);
    ET_SendEventReturn(renderTextBox, renderId, &ETRenderTextLogic::ET_getTextAABB);
    auto renderBoxSize = renderTextBox.getSize();
    Vec2i size(static_cast<int>(renderBoxSize.x), static_cast<int>(renderBoxSize.y));
    return size;
}