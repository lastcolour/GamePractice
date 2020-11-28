#include "UIUtils.hpp"
#include "Core/ETApplication.hpp"
#include "Config/UIConfig.hpp"
#include "UI/UILayoutStyle.hpp"
#include "Core/ETPrimitives.hpp"
#include "Entity/ETEntity.hpp"
#include "UI/ETUIViewPort.hpp"
#include "UI/ETUIBox.hpp"
#include "Logics/UIAnimationSequence.hpp"

#include <cassert>

namespace UI {

UIBoxMargin CalculateMargin(EntityId entityId, const UIBoxStyle::Margin& margin) {
    Transform tm;
    ET_SendEventReturn(tm, entityId, &ETEntity::ET_getTransform);

    UIBoxMargin resMargin;

    auto uiConfig = ET_getShared<UIConfig>();
    resMargin.left = uiConfig->getSizeOnGrind(margin.left);
    resMargin.right = uiConfig->getSizeOnGrind(margin.right);
    resMargin.bot = uiConfig->getSizeOnGrind(margin.bot);
    resMargin.top = uiConfig->getSizeOnGrind(margin.top);

    const auto& scale = tm.scale;
    resMargin.bot = static_cast<int>(resMargin.bot * scale.y);
    resMargin.top = static_cast<int>(resMargin.top * scale.y);
    resMargin.left = static_cast<int>(resMargin.left * scale.x);
    resMargin.right = static_cast<int>(resMargin.right * scale.x);

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

Vec2i CalcAligmentCenter(UIXAlign xAlign, UIYAlign yAlign, AABB2Di& parentBox, AABB2Di& box) {
    Vec2i center(0);
    switch(xAlign) {
        case UIXAlign::Left: {
            center.x = parentBox.bot.x + box.getSize().x / 2;
            break;
        }
        case UIXAlign::Center: {
            center.x = parentBox.getCenter().x;
            break;
        }
        case UIXAlign::Right: {
            center.x = parentBox.top.x - box.getSize().x / 2;
            break;
        }
        default: {
            assert(false && "Invalid x-aligment type");
        }
    }
    switch(yAlign) {
        case UIYAlign::Bot: {
            center.y = parentBox.bot.y + box.getSize().y / 2;
            break;
        }
        case UIYAlign::Center: {
            center.y = parentBox.getCenter().y;
            break;
        }
        case UIYAlign::Top: {
            center.y = parentBox.top.y - box.getSize().y / 2;
            break;
        }
        default: {
            assert(false && "Invalid y-aligment type");
        }
    }
    return center;
}

void Set2DPositionDoNotUpdateLayout(EntityId elemId, const Vec2i& pos) {
    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, true);

    Transform tm;
    ET_SendEventReturn(tm, elemId, &ETEntity::ET_getTransform);

    tm.pt.x = static_cast<float>(pos.x);
    tm.pt.y = static_cast<float>(pos.y);

    ET_SendEvent(elemId, &ETEntity::ET_setTransform, tm);

    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, false);
}

void SetTMDoNotUpdateLayout(EntityId elemId, const Transform& tm) {
    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, true);
    ET_SendEvent(elemId, &ETEntity::ET_setTransform, tm);
    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, false);
}

void SetLocalTMDoNotUpdateLayout(EntityId elemId, const Transform& tm) {
    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, true);
    ET_SendEvent(elemId, &ETEntity::ET_setLocalTransform, tm);
    ET_SendEvent(elemId, &ETUIElement::ET_setIgnoreTransform, false);
}

int GetValueOnGrind(float val) {
    auto uiConfig = ET_getShared<UIConfig>();
    return uiConfig->getSizeOnGrind(val);
}

float CovertValueFromGrid(int val) {
    auto uiConfig = ET_getShared<UIConfig>();
    return uiConfig->convertFromGrid(val);
}

Vec2i CalculateBoxSize(const UIBoxStyle& style) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i resSize(0);

    switch (style.widthInv)
    {
    case UIBoxSizeInvariant::Grid:
        resSize.x = UI::GetValueOnGrind(style.width);
        break;
    case UIBoxSizeInvariant::Relative:
        resSize.x = static_cast<int>(viewPort.x * style.width);
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
        resSize.y = static_cast<int>(viewPort.y * style.height);
        break;
    default:
        assert(false && "Invalid size invariant");
    }
    return resSize;
}

int GetZIndexForChild(EntityId entityId) {
    int zIndex = 0;
    int zIndexDepth = 0;
    ET_SendEventReturn(zIndex, entityId, &ETUIElement::ET_getZIndex);
    ET_SendEventReturn(zIndexDepth, entityId, &ETUIElement::ET_getZIndexDepth);
    int childZIndex = zIndex + zIndexDepth + 1;
    return childZIndex;
}

AABB2Di GetTmScaledBox(EntityId entityId, const AABB2Di& box) {
    Transform tm;
    ET_SendEventReturn(tm, entityId, &ETEntity::ET_getTransform);
    const auto& scale = tm.scale;

    AABB2Di resBox;
    resBox.bot = Vec2i(0);
    resBox.top = box.getSize();
    resBox.top.x = static_cast<int>(resBox.top.x * scale.x);
    resBox.top.y = static_cast<int>(resBox.top.y * scale.y);
    resBox.setCenter(box.getCenter());

    return resBox;
}

AABB2Di SetTmCenterToBox(EntityId entityId, const AABB2Di& box) {
    Transform tm;
    ET_SendEventReturn(tm, entityId, &ETEntity::ET_getTransform);

    auto resBox = box;

    Vec2i center = Vec2i(static_cast<int>(tm.pt.x),
        static_cast<int>(tm.pt.y));
    resBox.setCenter(center);

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

} // namespace UI
