#include "UI/UIConfigManager.hpp"

UIConfigManager::UIConfigManager() {

}
UIConfigManager::~UIConfigManager() {
}

bool UIConfigManager::init() {
    ETNode<ETUIConfigManager>::connect(getEntityId());
    return true;
}
void UIConfigManager::deinit() {
    ETNode<ETUIConfigManager>::disconnect();
}

const char* UIConfigManager::ET_getDefaultGeomRenderer() {
    return "";
}

const char* UIConfigManager::ET_getDefaultTextRenderer() {
    return "";
}