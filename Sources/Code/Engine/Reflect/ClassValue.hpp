#ifndef __CLASS_VALUE_HPP__
#define __CLASS_VALUE_HPP__

#include "Core/TypeId.hpp"

#include <string>
#include <functional>
#include <type_traits>

class ReflectContext;

enum class ClassValueType {
    Invalid = 0,
    Bool,
    Int,
    Float,
    String,
    Object
};

template<typename ValueT>
constexpr ClassValueType getClassValueType() {
    if constexpr (std::is_same<ValueT, bool>::value) {
        return ClassValueType::Bool;
    } else if constexpr (std::is_same<ValueT, int>::value) {
        return ClassValueType::Int;
    } else if constexpr(std::is_same<ValueT, float>::value) {
        return ClassValueType::Float;
    } else if constexpr (std::is_same<ValueT, std::string>::value) {
        return ClassValueType::String;
    } else if constexpr (std::is_function<decltype(ValueT::reflect)>::value
        && std::is_same<decltype(&ValueT::reflect), void(*)(ReflectContext&)>::value) {
        return ClassValueType::Object;
    }
    return ClassValueType::Invalid;
}

class ClassValue {
public:

    using ValuePtrT = void* ClassValue::*;

public:

    std::string name;
    ClassValueType type;
    ValuePtrT ptr;
    TypeId typeId;
};

#endif /* __CLASS_VALUE_HPP__ */