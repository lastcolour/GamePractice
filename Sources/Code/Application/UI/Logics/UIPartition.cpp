#include "UI/Logics/UIPartition.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

const char* DEFAULT_LIST = "UI/Default/List.json";
const char* DEFAULT_BOX = "UI/Default/Box.json";

} // namespace

UIPartition::UIPartition() {
}

UIPartition::~UIPartition() {
}

bool UIPartition::serialize(const JSONNode& node) {
    auto childId = serializeNode(node);
    if(childId.isValid()) {
        ET_SendEvent(getEntityId(), &ETGameObject::ET_addChild, childId);
    }
    return true;
}

bool UIPartition::init() {
    UIBaseBox::init();
    return true;
}

EntityId UIPartition::serializeNode(const JSONNode& node) {
    std::string typeVal;
    node.value("type", typeVal);
    if(typeVal == "vert") {
        return serializeAsList(UIListType::Vertical, node);
    } else if(typeVal == "horz") {
        return serializeAsList(UIListType::Horizontal, node);
    } else if(typeVal == "box") {
        return serializeAsBox(node);
    } else if(typeVal == "object") {
        return serializeAsObject(node);
    } else if (typeVal.empty()) {
        return serializeAsSimplified(node);
    }
    LogWarning("[UIPartition::serializeNode] Unknown node type: %s", typeVal);
    return InvalidEntityId;
}

void UIPartition::serializeChildren(EntityId rootEntId, const JSONNode& node) {
    for(auto& childNode : node.object("children")) {
        auto childId = serializeNode(childNode);
        if(childId.isValid()) {
            ET_SendEvent(rootEntId, &ETGameObject::ET_addChild, childId);
        }
    }
}

EntityId UIPartition::serializeAsSimplified(const JSONNode& node) {
    if(!node.object("style")) {
        return serializeAsObject(node);
    }
    std::string objVal;
    node.value("object", objVal);
    if(objVal.empty()) {
        return serializeAsBox(node);
    }
    if(node.object("children")) {
        LogWarning("[UIPartition::serializeNode] Can't use simplified syntext for object inside box with children");
        return InvalidEntityId;
    }
    EntityId boxId = serializeAsBox(node);
    if(boxId.isValid()) {
        EntityId objId = serializeAsObject(node);
        if(objId.isValid()) {
            ET_SendEvent(boxId, &ETGameObject::ET_addChild, objId);
        }
    }
    return boxId;
}

EntityId UIPartition::serializeAsBox(const JSONNode& node) {
    EntityId boxId;
    ET_SendEventReturn(boxId, &ETGameObjectManager::ET_createGameObject, DEFAULT_BOX);
    if(!boxId.isValid()) {
        LogWarning("[UIPartition::serializeAsObject] Can't create default box");
        return InvalidEntityId;
    }

    if(auto styleNode = node.object("style")) {
        UIStyle style;
        style.serialize(styleNode);
        ET_SendEvent(boxId, &ETUIBox::ET_setStyle, style);
    }

    serializeChildren(boxId, node);

    return boxId;
}

EntityId UIPartition::serializeAsList(UIListType listType, const JSONNode& node) {
    EntityId listId;
    ET_SendEventReturn(listId, &ETGameObjectManager::ET_createGameObject, DEFAULT_LIST);
    if(!listId.isValid()) {
        LogWarning("[UIPartition::serializeAsList] Can't create default list");
        return InvalidEntityId;
    }

    ET_SendEvent(listId, &ETUIList::ET_setType, listType);

    if(auto styleNode = node.object("style")) {
        UIStyle style;
        style.serialize(styleNode);
        ET_SendEvent(listId, &ETUIBox::ET_setStyle, style);
    }

    serializeChildren(listId, node);

    return listId;
}

EntityId UIPartition::serializeAsObject(const JSONNode& node) {
    std::string objectVal;
    node.value("object", objectVal);

    EntityId objId;
    ET_SendEventReturn(objId, &ETGameObjectManager::ET_createGameObject, objectVal.c_str());
    if(!objId.isValid()) {
        LogWarning("[UIPartition::serializeAsObject] Can't create object: %s", objectVal);
        return InvalidEntityId;
    }

    serializeChildren(objId, node);

    return objId;
}