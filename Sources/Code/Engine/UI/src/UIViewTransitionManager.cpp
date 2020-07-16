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

void UIViewTransitionManager::ET_onViewAppeared(EntityId viewId) {
    if(tasks.empty()) {
        return;
    }
    auto& currentTask = tasks.front();
    if(currentTask.viewId == viewId) {
        assert(currentTask.isAppearing && "Invalid task type");
        tasks.erase(tasks.begin());
        startNextTask();
    }
}

void UIViewTransitionManager::ET_onViewDisappeared(EntityId viewId) {
    if(tasks.empty()) {
        return;
    }
    auto& currentTask = tasks.front();
    if(currentTask.viewId == viewId) {
        assert(!currentTask.isAppearing && "Invalid task type");
        tasks.erase(tasks.begin());
        startNextTask();
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
    if(tasks.empty()) {
        return;
    }
    auto& task = tasks.front();
    if(!ET_IsExistNode<ETUIViewAppearAnimation>(task.viewId)) {
        if(task.isAppearing) {
            ET_SendEvent(task.viewId, &ETUIElement::ET_show);
        } else {
            ET_SendEvent(task.viewId, &ETUIElement::ET_hide);
        }
        tasks.pop_back();
    } else {
        if(task.isAppearing) {
            ET_SendEvent(task.viewId, &ETUIViewAppearAnimation::ET_appear);
        } else {
            ET_SendEvent(task.viewId, &ETUIViewAppearAnimation::ET_disappear);
        }
    }
}
