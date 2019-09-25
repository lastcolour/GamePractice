#include "UI/Logics/UIBox.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

const char* DEFAULT_LABEL = "UI/Default/Label.json";
const char* DEFAULT_IMAGE = "UI/Default/Image.json";
const char* DEFAULT_GEOM_RENDERER = "Render/RenderGeomSimple.json";

EntityId createLabel(const JSONNode& node) {
    EntityId entId;
    std::string content;
    node.read("text", content);
    if(!content.empty()) {
        ET_SendEventReturn(entId, &ETEntityManager::ET_createEntity, DEFAULT_LABEL);
        if(!entId.isValid()) {
            return entId;
        }
        ET_SendEvent(entId, &ETUILabel::ET_setText, content.c_str());
    } else {
        node.read("image", content);
        if(!content.empty()) {
            ET_SendEventReturn(entId, &ETEntityManager::ET_createEntity, DEFAULT_IMAGE);
            if(!entId.isValid()) {
                return entId;
            }
            ET_SendEvent(entId, &ETUIImage::ET_setImage, content.c_str());
        }
    }
    if(auto styleNode = node.object("style")) {
        UIStyle style;
        ET_SendEventReturn(style, entId, &ETUIBox::ET_getStyle);
        style.serialize(styleNode);
        ET_SendEvent(entId, &ETUIBox::ET_setStyle, style);
    }
    return entId;
}

} // namespace

UIBox::UIBox() {
}

UIBox::~UIBox() {
}

bool UIBox::serialize(const JSONNode& node) {
    UIBaseBox::serialize(node);
    if(auto labelNode = node.object("label")) {
        labelId = createLabel(labelNode);
    }
    return true;
}

bool UIBox::init() {
    UIBaseBox::init();
    createRenderer();
    updateRenderParams();
    updateRenderSize();
    if(labelId.isValid()) {
        ET_SendEvent(getEntityId(), &ETEntity::ET_addChild, labelId);
    }
    ETNode<ETUILabeledBox>::connect(getEntityId());
    return true;
}

EntityId UIBox::ET_getLabelId() const {
    return labelId;
}

void UIBox::ET_setLabelText(const char* text) {
    if(!labelId.isValid()) {
        LogWarning("[UIBox::ET_setLabelText] Can't set label text without label");
        return;
    }
    ET_SendEvent(labelId, &ETUILabel::ET_setText, text);
}

void UIBox::ET_setStyle(const UIStyle& newStyle) {
    bool isNewRenderer = false;
    if(ET_getStyle().renderer != newStyle.renderer && !newStyle.renderer.empty()) {
        isNewRenderer = true;
    }
    UIBaseBox::ET_setStyle(newStyle);
    if (isNewRenderer) {
        createRenderer();
    }
    updateRenderParams();
}

void UIBox::ET_boxResize() {
    UIBaseBox::ET_boxResize();
    updateRenderSize();
}

EntityId UIBox::getRendererId() const {
    return renderId;
}

void UIBox::createRenderer() {
    if(renderId.isValid()) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, renderId);
        renderId = InvalidEntityId;
    }
    const auto& boxStyle = ET_getStyle();
    std::string rendererName = boxStyle.renderer;
    if(rendererName.empty() && boxStyle.color != ColorB(0, 0, 0, 0)) {
       rendererName = DEFAULT_GEOM_RENDERER;
    } else {
        return;
    }
    ET_SendEventReturn(renderId, &ETEntityManager::ET_createEntity, rendererName.c_str());
    if(renderId.isValid()) {
        ET_SendEvent(renderId, &ETEntity::ET_setParent, getEntityId());
        ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setColor, boxStyle.color);
        Transform tm;
        ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
        ET_SendEvent(renderId, &ETEntity::ET_setTransform, tm);
    } else {
        LogWarning("[UIBox::createRenderer] Can't create renderer: %s", rendererName);
    }
}

void UIBox::updateRenderSize() {
    if(!renderId.isValid()) {
        return;
    }
    const auto& uiBox = ET_getAabb2di();
    auto uiBoxSize = uiBox.getSize();
    ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setSize,
        Vec2(static_cast<float>(uiBoxSize.x), static_cast<float>(uiBoxSize.y)));
}

void UIBox::updateRenderParams() {
    if(!renderId.isValid()) {
        return;
    }
    ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setColor, ET_getStyle().color);
}