#include "UIUtils.hpp"
#include "Core/ETApplication.hpp"
#include "Config/UIConfig.hpp"
#include "UI/UILayoutStyle.hpp"
#include "UI/ETUIViewPort.hpp"
#include "UI/ETUIBox.hpp"
#include "Logics/UIAnimationSequence.hpp"
#include "Core/GlobalData.hpp"

#include <cassert>

namespace UI {

UIBoxMargin CalculateMargin(EntityId entityId, const UIBoxStyle::Margin& margin) {
    Transform tm;
    ET_SendEventReturn(tm, entityId, &ETEntity::ET_getTransform);

    UIBoxMargin resMargin;

    auto uiConfig = GetGlobal<UIConfig>();
    resMargin.left = uiConfig->getSizeOnGrid(margin.left);
    resMargin.right = uiConfig->getSizeOnGrid(margin.right);
    resMargin.bot = uiConfig->getSizeOnGrid(margin.bot);
    resMargin.top = uiConfig->getSizeOnGrid(margin.top);

    const auto& scale = tm.scale;
    resMargin.bot *= scale.y;
    resMargin.top *= scale.y;
    resMargin.left *= scale.x;
    resMargin.right *= scale.x;

    return resMargin;
}

const char* GetViewTypeName(UIViewType viewType) {
    const char* viewTypeStr = "Invalid";
    switch(viewType) {
        case UIViewType::None: {
            viewTypeStr = "None";
            break;
        }
        case UIViewType::Main: {
            viewTypeStr = "Main";
            break;
        }
        case UIViewType::EndGame: {
            viewTypeStr = "EndGame";
            break;
        }
        case UIViewType::Game: {
            viewTypeStr = "Game";
            break;
        }
        case UIViewType::PauseGame: {
            viewTypeStr = "PauseGame";
            break;
        }
        case UIViewType::Background: {
            viewTypeStr = "Background";
            break;
        }
        case UIViewType::Loading: {
            viewTypeStr = "Loading";
            break;
        }
        case UIViewType::Levels: {
            viewTypeStr = "Levels";
            break;
        }
        default: {
            assert(false && "INvalid view type");
            break;
        }
    }
    return viewTypeStr;
}

Vec2 CalcAligmentCenter(UIXAlign xAlign, UIYAlign yAlign, const AABB2D& parentBox, const AABB2D& box) {
    Vec2 center(0.f);
    switch(xAlign) {
        case UIXAlign::Left: {
            center.x = parentBox.bot.x + box.getSize().x / 2.f;
            break;
        }
        case UIXAlign::Center: {
            center.x = parentBox.getCenter().x;
            break;
        }
        case UIXAlign::Right: {
            center.x = parentBox.top.x - box.getSize().x / 2.f;
            break;
        }
        default: {
            assert(false && "Invalid x-aligment type");
        }
    }
    switch(yAlign) {
        case UIYAlign::Bot: {
            center.y = parentBox.bot.y + box.getSize().y / 2.f;
            break;
        }
        case UIYAlign::Center: {
            center.y = parentBox.getCenter().y;
            break;
        }
        case UIYAlign::Top: {
            center.y = parentBox.top.y - box.getSize().y / 2.f;
            break;
        }
        default: {
            assert(false && "Invalid y-aligment type");
        }
    }
    return center;
}

void Set2DPositionDoNotUpdateLayout(EntityId elemId, const Vec2& pos) {
    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, true);

    Transform tm;
    ET_SendEventReturn(tm, elemId, &ETEntity::ET_getTransform);

    tm.pt.x = pos.x;
    tm.pt.y = pos.y;

    ET_SendEvent(elemId, &ETEntity::ET_setTransform, tm);

    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, false);
}

void SetTMDoNotUpdateLayout(EntityId elemId, const Transform& tm) {
    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, true);
    ET_SendEvent(elemId, &ETEntity::ET_setTransform, tm);
    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, false);
}

float GetValueOnGrind(float val) {
    auto uiConfig = GetGlobal<UIConfig>();
    return uiConfig->getSizeOnGrid(val);
}

float ConvertValueFromGrid(float val) {
    auto uiConfig = GetGlobal<UIConfig>();
    return uiConfig->convertFromGrid(val);
}

Vec2 CalculateBoxSize(const UIBoxStyle& style) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    Vec2 resSize(0.f);
    switch (style.widthInv)
    {
    case UIBoxSizeInvariant::Grid:
        resSize.x = UI::GetValueOnGrind(style.width);
        break;
    case UIBoxSizeInvariant::Relative:
        resSize.x = viewPort.x * style.width;
        break;
    default:
        assert(false && "Invalid size invariant");
    }
    switch (style.heightInv)
    {
    case UIBoxSizeInvariant::Grid:
        resSize.y = UI::GetValueOnGrind(style.height);
        break;
    case UIBoxSizeInvariant::Relative:
        resSize.y = viewPort.y * style.height;
        break;
    default:
        assert(false && "Invalid size invariant");
    }
    return resSize;
}

AABB2D SetTmCenterToBox(EntityId entityId, const AABB2D& box) {
    Transform tm;
    ET_SendEventReturn(tm, entityId, &ETEntity::ET_getTransform);

    auto resBox = box;
    resBox.setCenter(tm.pt.x, tm.pt.y);

    return resBox;
}

AABB2D ApplyEntityTmToBox(EntityId entityId, const AABB2D& box) {
    Transform tm;
    ET_SendEventReturn(tm, entityId, &ETEntity::ET_getTransform);

    const auto& scale = tm.scale;

    AABB2D resBox;

    resBox.bot = Vec2(0.f);
    resBox.top = box.getSize();
    resBox.top.x *= scale.x;
    resBox.top.y *= scale.y;

    resBox.setCenter(tm.pt.x, tm.pt.y);

    return resBox;
}

bool IsRootViewHasFocus(EntityId elemId) {
    EntityId hostLayoutId;
    while(true) {
        ET_SendEventReturn(hostLayoutId, elemId, &ETUIElement::ET_getHostLayout);
        if(!hostLayoutId.isValid()) {
            return true;
        }
        if(!ET_IsExistNode<ETUIView>(hostLayoutId)) {
            elemId = hostLayoutId;
        } else {
            bool hasFocus = false;
            ET_SendEventReturn(hasFocus, hostLayoutId, &ETUIView::ET_getFocus);
            return hasFocus;
        }
    }
    return false;
}

UIAnimationSequence* GetAnimation(EAnimSequenceType animType, EntityId entityId) {
    auto animations = GetEntityLogics<UIAnimationSequence>(entityId);
    for(auto anim : animations) {
        if(anim->ET_getType() == animType) {
            return anim;
        }
    }
    return nullptr;
}

void CopyUIElemAttribsFromParent(EntityId fromId, EntityId toId) {
    bool isEnabled = false;
    ET_SendEventReturn(isEnabled, fromId, &ETUIElement::ET_isEnabled);
    ET_SendEvent(toId, &ETUIElement::ET_setParentDisabled, !isEnabled);

    bool isHidden = false;
    ET_SendEventReturn(isHidden, fromId, &ETUIElement::ET_isHidden);
    ET_SendEvent(toId, &ETUIElement::ET_setParentHidden, isHidden);

    float alpha = 1.f;
    ET_SendEventReturn(alpha, fromId, &ETUIElement::ET_getAlpha);
    ET_SendEvent(toId, &ETUIElement::ET_setParentAlpha, alpha);
}

} // namespace UI
