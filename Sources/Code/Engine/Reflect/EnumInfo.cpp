#include "Reflect/EnumInfo.hpp"
#include "ETApplicationInterfaces.hpp"

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

bool EnumInfo::serializeValue(void* valuePtr, const std::string& valueStr) const {
    if(valueStr.empty()) {
        LogError("[EnumInfo::serializeValue] Can't serialize value of enum '%s' (Error: %s)",
            enumName, "value empty");
        return false;
    }
    auto it = nameToVal.find(valueStr);
    if(it == nameToVal.end()) {
        LogError("[EnumInfo::serializeValue] Can't serialize value of enum '%s' (Error: %s)",
            enumName, "can't find value");
        return false;
    }
    *(static_cast<int*>(valuePtr)) = it->second;
    return true;
}