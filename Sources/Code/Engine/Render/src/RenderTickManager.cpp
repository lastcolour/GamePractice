#include "RenderTickManager.hpp"

RenderTickManager::RenderTickManager() {
}

RenderTickManager::~RenderTickManager() {
}

bool RenderTickManager::init() {
    ETNode<ETRenderTickManager>::connect(getEntityId());
    return true;
}

void RenderTickManager::deinit() {
}

void RenderTickManager::ET_onRenderTick(float dt) {
    curr.renderDt += dt;
}

void RenderTickManager::ET_onGameTick(float dt) {
    std::lock_guard<std::mutex> lock(mutex);
    curr.gameDt += dt;
}

void RenderTickManager::ET_onUITick(float dt) {
    std::lock_guard<std::mutex> lock(mutex);
    curr.uiDt += dt;
}

ETRenderTickManager::Ticks RenderTickManager::ET_getTicks() const {
    return prev;
}

void RenderTickManager::ET_fetchDeltaT() {
    std::lock_guard<std::mutex> lock(mutex);
    prev = curr;
    curr.renderDt = 0.f;
    curr.gameDt = 0.f;
    curr.uiDt = 0.f;
}