#include "UI/Logics/UIBox.hpp"
#include "Core/JSONNode.hpp"
#include "Game/GameObjectManager.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

const char* DEFAULT_LABEL_ID = "UI/Default/Label.json";
const char* DEFAULT_GEOM_RENDERER = "Render/RenderGeomSimple.json";

EntityId createLabel(const JSONNode& node) {
    EntityId entId;
    ET_SendEventReturn(entId, &ETGameObjectManager::ET_createGameObject, DEFAULT_LABEL_ID);
    if(!entId.isValid()) {
        return entId;
    }
    if(auto styleNode = node.object("style")) {
        UIStyle style;
        ET_SendEventReturn(style, entId, &ETUIBox::ET_getStyle);
        style.serialize(styleNode);
        ET_SendEvent(entId, &ETUIBox::ET_setStyle, style);
    }
    std::string labelText;
    node.value("text", labelText);
    ET_SendEvent(entId, &ETUILabel::ET_setText, labelText.c_str());
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
        if(labelId.isValid()) {
            ET_SendEvent(getEntityId(), &ETGameObject::ET_addChild, labelId);
        }
    }
    return true;
}

bool UIBox::init() {
    UIBaseBox::init();
    createRenderer();
    updateRenderParams();
    updateRenderSize();
    return true;
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
        ET_SendEvent(&ETGameObjectManager::ET_destroyObject, renderId);
        renderId = InvalidEntityId;
    }
    const auto& boxStyle = ET_getStyle();
    std::string rendererName = boxStyle.renderer;
    if(rendererName.empty()) {
       rendererName = DEFAULT_GEOM_RENDERER;
    }
    ET_SendEventReturn(renderId, &ETGameObjectManager::ET_createGameObject, rendererName.c_str());
    if(renderId.isValid()) {
        ET_SendEvent(renderId, &ETGameObject::ET_setParent, getEntityId());
        ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setColor, boxStyle.color);
        Transform tm;
        ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);
        ET_SendEvent(renderId, &ETGameObject::ET_setTransform, tm);
    } else {
        LogWarning("[UIBox::createRenderer] Can't create renderer: %s", rendererName);
    }
}

void UIBox::updateRenderSize() {
    if(!renderId.isValid()) {
        return;
    }
    const auto& box = ET_getAabb2di();
    auto boxSize = box.getSize();
    ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setSize,
        Vec2(static_cast<float>(boxSize.x), static_cast<float>(boxSize.y)));
}

void UIBox::updateRenderParams() {
    if(!renderId.isValid()) {
        return;
    }
    ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setColor, ET_getStyle().color);
}