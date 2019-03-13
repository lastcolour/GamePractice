#include "UI/Logics/UIList.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

UIList::UIList() {
}

UIList::~UIList() {
}

void UIList::addElement(EntityId entId) {

}

bool UIList::serialize(const JSONNode& node) {
    if(!UIBox::serialize(node)) {
        LogWarning("[UIList::serialize] UIBox searilization failed");
        return false;
    }
    auto childrenNode = node.object("children");
    if(!childrenNode) {
        LogWarning("[UIList::serialize] Can't find required children node");
        return false;
    }
    for(auto& childNode : childrenNode) {
        std::string childObjName;
        childrenNode.value(childObjName);
        EntityId childEntId;
        ET_SendEventReturn(childEntId, &ETGame::ET_createGameObject, childObjName);
        if(childEntId != InvalidEntityId) {
            ET_SendEvent(&ETGameObject::ET_addChild, childEntId);
            children.push_back(childEntId);
        }
    }
}

bool UIList::init() {
    if(!UIBox::init()) {
        LogWarning("[UIList::init] UIBox init failed");
        return false;
    }

    for(auto entId : children) {
        addElement(entId);
    }

    return true;
}