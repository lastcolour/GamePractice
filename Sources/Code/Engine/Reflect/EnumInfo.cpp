#include "Reflect/EnumInfo.hpp"
#include "ETApplicationInterfaces.hpp"

EnumInfo::EnumInfo(const char* name, TypeId typeId) :
    enumName(name),
    enumTypeId(typeId) {
}

EnumInfo::~EnumInfo() {
}

bool EnumInfo::addEnumPair(const std::string& name, int val) {
    if(name.empty()) {
        return false;
    }
    auto it = nameToVal.find(name);
    if(it != nameToVal.end()) {
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
        return false;
    }
    auto it = nameToVal.find(valueStr);
    if(it == nameToVal.end()) {
        return false;
    }
    *(static_cast<int*>(valuePtr)) = it->second;
    return true;
}