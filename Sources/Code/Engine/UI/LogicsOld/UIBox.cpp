#include "UI/Logics/UIBox.hpp"
#include "ETApplicationInterfaces.hpp"

UIBox::UIBox() {
}

UIBox::~UIBox() {
}

void UIBox::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIBox>("UIBox")) {
        classInfo->addBaseClass<UIBaseBox>();
    }
}

bool UIBox::init() {
    UIBaseBox::init();
    createRenderer();
    updateRenderParams();
    updateRenderSize();
    ETNode<ETUILabeledBox>::connect(getEntityId());
    return true;
}

void UIBox::deinit() {
}

void UIBox::ET_setBoxStyle(const UIBoxStyle& newStyle) {
    bool isNewRenderer = false;
    if(ET_getBoxStyle().renderer != newStyle.renderer && !newStyle.renderer.empty()) {
        isNewRenderer = true;
    }
    UIBaseBox::ET_setBoxStyle(newStyle);
    if(isNewRenderer) {
        createRenderer();
    }
    updateRenderParams();
}

void UIBox::ET_boxResize() {
    UIBaseBox::ET_boxResize();
    updateRenderSize();
}

void UIBox::createRenderer() {
    if(renderId.isValid()) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, renderId);
        renderId = InvalidEntityId;
    }
    const auto& boxStyle = ET_getBoxStyle();
    std::string rendererName = boxStyle.renderer;
    if(rendererName.empty()) {
        return;
    }
    ET_SendEventReturn(renderId, &ETEntityManager::ET_createEntity, rendererName.c_str());
    if(renderId.isValid()) {
        ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setColor, boxStyle.color);
        setUpRenderChild(renderId);
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
    ET_SendEvent(renderId, &ETRenderRect::ET_setSize, uiBoxSize);
}

void UIBox::updateRenderParams() {
    if(!renderId.isValid()) {
        return;
    }
    ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setColor, ET_getBoxStyle().color);
}

EntityId UIBox::getRenderId() const {
    return renderId;
}