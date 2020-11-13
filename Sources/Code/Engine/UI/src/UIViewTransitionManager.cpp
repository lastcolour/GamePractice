#include "UIViewTransitionManager.hpp"
#include "UI/ETUIView.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIBox.hpp"

#include <cassert>

UIViewTransitionManager::UIViewTransitionManager() {
}

UIViewTransitionManager::~UIViewTransitionManager() {
}

bool UIViewTransitionManager::init() {
    ETNode<ETUIViewAppearAnimationEvents>::connect(getEntityId());
    ETNode<ETUIViewTransitionManager>::connect(getEntityId());
    return true;
}

void UIViewTransitionManager::deinit() {
}

void UIViewTransitionManager::ET_onAppearPlayed(EntityId viewId) {
    assert(!tasks.empty() && "Empty tasks");
    auto& currentTask = tasks.front();
    if(currentTask.viewId == viewId) {
        assert(currentTask.isAppearing && "Invalid task type");
        tasks.erase(tasks.begin());
        ET_SendEvent(&ETUIViewManager::ET_onViewAppeared, viewId);
        startNextTask();
    } else {
        assert(false && "Invalid viewId");
    }
}

void UIViewTransitionManager::ET_onDisappearPlayed(EntityId viewId) {
    assert(!tasks.empty() && "Empty tasks");
    auto& currentTask = tasks.front();
    if(currentTask.viewId == viewId) {
        assert(!currentTask.isAppearing && "Invalid task type");
        tasks.erase(tasks.begin());
        ET_SendEvent(&ETUIViewManager::ET_onViewDisappeared, viewId);
        startNextTask();
    } else {
        assert(false && "Invalid viewId");
    }
}

void UIViewTransitionManager::ET_addAppearing(EntityId viewId) {
    AppearTask task;
    task.viewId = viewId;
    task.isAppearing = true;
    tasks.push_back(task);
    if(tasks.size() == 1) {
        startNextTask();
    }
}

void UIViewTransitionManager::ET_addDisappearing(EntityId viewId) {
    AppearTask task;
    task.viewId = viewId;
    task.isAppearing = false;
    tasks.push_back(task);
    if(tasks.size() == 1) {
        startNextTask();
    }
}

void UIViewTransitionManager::startNextTask() {
    while(!tasks.empty()) {
        auto task = tasks.front();
        if(!ET_IsExistNode<ETUIViewAppearAnimation>(task.viewId)) {
            tasks.erase(tasks.begin());
            if(task.isAppearing) {
                ET_SendEvent(task.viewId, &ETUIElement::ET_show);
                ET_SendEvent(&ETUIViewManager::ET_onViewStartAppearing, task.viewId);
                ET_SendEvent(&ETUIViewManager::ET_onViewAppeared, task.viewId);
            } else {
                ET_SendEvent(task.viewId, &ETUIElement::ET_hide);
                ET_SendEvent(&ETUIViewManager::ET_onViewDisappeared, task.viewId);
            }
        } else {
            if(task.isAppearing) {
                ET_SendEvent(&ETUIViewManager::ET_onViewStartAppearing, task.viewId);
                ET_SendEvent(task.viewId, &ETUIViewAppearAnimation::ET_playAppear, getEntityId());
            } else {
                ET_SendEvent(task.viewId, &ETUIViewAppearAnimation::ET_playDissapear, getEntityId());
            }
            break;
        }
    }
}

bool UIViewTransitionManager::ET_hasActiveTransition() const {
    return !tasks.empty();
}