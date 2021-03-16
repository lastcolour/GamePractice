#ifndef __ENUM_INFO_HPP__
#define __ENUM_INFO_HPP__

#include "Core/TypeId.hpp"

#include <type_traits>

class JSONNode;
class MemoryStream;

class EnumInfo {
public:

    EnumInfo(const char* name, TypeId typeId);
    ~EnumInfo();

    const char* getName() const;
    TypeId getEnumTypeId() const;
    bool getValueFromString(const std::string& valueStr, int& outValue) const;
    bool getStringFromValue(int value, std::string& outStrValue) const;
    int getDefaultValue();
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

    std::unordered_map<std::string, int> nameToVal;
    std::string enumName;
    TypeId enumTypeId;
};

#endif /* __ENUM_INFO_HPP__ */