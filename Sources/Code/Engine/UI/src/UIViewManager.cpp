#include "UIViewManager.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntityManger.hpp"

#include <cassert>

namespace {

const char* MAIN_VIEW = "UI/MainView/MainView.json";
const char* GAME_VIEW = "UI/GameView/GameView.json";

} // namespace

UIViewManager::UIViewManager() {
}

UIViewManager::~UIViewManager() {
}

bool UIViewManager::init() {
    ETNode<ETUIViewManager>::connect(getEntityId());
    ETNode<ETUIViewAppearAnimationEvents>::connect(getEntityId());
    return true;
}

void UIViewManager::deinit() {
}

void UIViewManager::ET_onViewAppeared() {
}

void UIViewManager::ET_onViewDisappeared() {
}

bool UIViewManager::ET_openView(UIViewType viewType) {
    switch(viewType) {
        case UIViewType::Main: {
            EntityId viewId;
            ET_SendEventReturn(viewId, &ETEntityManager::ET_createEntity, MAIN_VIEW);
            if(!viewId.isValid()) {
                LogError("[UIViewManager::ET_openView] Can't create MAIN view: '%s'", MAIN_VIEW);
                return false;
            }
            UIViewNode node;
            node.type = UIViewType::Main;
            node.id = viewId;
            stack.push_back(node);
            return true;
        }
        case UIViewType::Game: {
            EntityId viewId;
            ET_SendEventReturn(viewId, &ETEntityManager::ET_createEntity, GAME_VIEW);
            if(!viewId.isValid()) {
                LogError("[UIViewManager::ET_openView] Can't create GAME view: '%s'", GAME_VIEW);
                return false;
            }
            UIViewNode node;
            node.type = UIViewType::Game;
            node.id = viewId;
            stack.push_back(node);
            return true;
        }
        case UIViewType::None: {
            [[fallthrough]];
        }
        default: {
            assert(false && "Invalid View Type");
        }
    }
    return false;
}
void UIViewManager::ET_closeView(UIViewType viewType) {
    auto it = std::find_if(stack.begin(), stack.end(), [viewType](const UIViewNode& node){
        return viewType == node.type;
    });
    if(it == stack.end()) {
        LogWarning("[UIViewManager::ET_closeView] Can't find view to close");
        return;
    }
    ET_SendEvent(&ETEntityManager::ET_destroyEntity, it->id);
    stack.erase(it);
}

UIViewType UIViewManager::ET_getActiveViewType() const {
    if(stack.empty()) {
        return UIViewType::None;
    }
    return stack.back().type;
}

EntityId UIViewManager::ET_getActiveViewId() const {
    if(stack.empty()) {
        return InvalidEntityId;
    }
    return stack.back().id;
}