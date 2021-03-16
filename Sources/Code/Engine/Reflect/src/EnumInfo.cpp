#include "Reflect/EnumInfo.hpp"
#include "Core/JSONNode.hpp"
#include "Core/MemoryStream.hpp"

#include <cassert>

EnumInfo::EnumInfo(const char* name, TypeId typeId) :
    enumName(name),
    enumTypeId(typeId) {
}

EnumInfo::~EnumInfo() {
}

bool EnumInfo::addEnumPair(const std::string& name, int val) {
    if(name.empty()) {
        LogError("[EnumInfo::addEnumPair] Can't add values pair for enum: '%s' (Error: empty name)",
            enumName);
        assert(false && "can't add enum value");
        return false;
    }
    auto it = nameToVal.find(name);
    if(it != nameToVal.end()) {
        LogError("[EnumInfo::addEnumPair] Can't add values pair <%s, %d> for enum: '%s' (Error: name already exists)",
            name, val, enumName);
        assert(false && "can't add enum value");
        return false;
    }
    nameToVal[name] = val;
    return true;
}

const char* EnumInfo::getName() const {
    return enumName.c_str();
}

TypeId EnumInfo::getEnumTypeId() const {
    return enumTypeId;
}

bool EnumInfo::getValueFromString(const std::string& valueStr, int& outValue) const {
    if(valueStr.empty()) {
        return false;
    }
    auto it = nameToVal.find(valueStr);
    if(it == nameToVal.end()) {
        return false;
    }
    outValue = it->second;
    return true;
}

bool EnumInfo::getStringFromValue(int value, std::string& outStrValue) const {
    for(auto& node : nameToVal) {
        if(node.second == value) {
            outStrValue = node.first;
            return true;
        }
    }
    return false;
}

int EnumInfo::getDefaultValue() {
    if(nameToVal.empty()) {
        return 0;
    }
    return nameToVal.begin()->second;
}

void EnumInfo::makeReflectModel(JSONNode& node) {
    node.write("type", "enum");
    JSONNode enumValsNode;
    for(auto& enumPair : nameToVal) {
        enumValsNode.write(enumPair.first.c_str(), static_cast<int>(enumPair.second));
    }
    node.write("data", enumValsNode);
}