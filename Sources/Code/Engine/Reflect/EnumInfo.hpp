#ifndef __ENUM_INFO_HPP__
#define __ENUM_INFO_HPP__

#include "Core/TypeId.hpp"

#include <type_traits>
#include <unordered_map>
#include <string>

class JSONNode;

class EnumInfo {
public:

    EnumInfo(const char* name, TypeId typeId);
    ~EnumInfo();

    const char* getName() const;
    TypeId getEnumTypeId() const;
    bool readValue(void* valuePtr, const std::string& valueStr) const;
    void makeReflectModel(JSONNode& node);

    template<typename EnumT>
    void addValues(const std::initializer_list<std::pair<std::string, EnumT>>& values) {
        static_assert(std::is_enum<EnumT>::value, "Not enum type");
        if(enumTypeId != GetTypeId<EnumT>()) {
            return;
        }
        for(auto& pair : values) {
           if(!addEnumPair(pair.first, static_cast<int>(pair.second))) {
               break;
           }
        }
    }

private:

    bool addEnumPair(const std::string& name, int val);

private:

    TypeId enumTypeId;
    std::string enumName;
    std::unordered_map<std::string, int> nameToVal;
};

#endif /* __ENUM_INFO_HPP__ */