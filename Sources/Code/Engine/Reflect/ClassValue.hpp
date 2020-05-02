#ifndef __CLASS_VALUE_HPP__
#define __CLASS_VALUE_HPP__

#include "Core/TypeId.hpp"
#include "Math/Vector.hpp"
#include "Render/Color.hpp"

#include <string>
#include <functional>
#include <type_traits>

namespace Reflect {

template<typename>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T,A>> : std::true_type {};

} // namespace


class ReflectContext;

enum class ClassValueType {
    Invalid = 0,
    Bool,
    Int,
    Float,
    String,
    Vec2i,
    Vec2,
    Vec3,
    Vec4,
    Color,
    Object,
    Resource,
    Enum,
    Array
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
    } else if constexpr (std::is_same<ValueT, Vec2i>::value) {
        return ClassValueType::Vec2i;
    } else if constexpr (std::is_same<ValueT, Vec2>::value) {
        return ClassValueType::Vec2;
    } else if constexpr (std::is_same<ValueT, Vec3>::value) {
        return ClassValueType::Vec3;
    } else if constexpr (std::is_same<ValueT, Vec4>::value) {
        return ClassValueType::Vec4;
    } else if constexpr (std::is_same<ValueT, ColorB>::value) {
        return ClassValueType::Color;
    } else if constexpr (Reflect::is_std_vector<ValueT>::value) {
        return ClassValueType::Array;
    } else if constexpr (std::is_function<decltype(ValueT::Reflect)>::value) {
        if constexpr (std::is_same<decltype(&ValueT::Reflect), void(*)(ReflectContext&)>::value) {
            return ClassValueType::Object;
        } else {
            return ClassValueType::Invalid;
        }
    } else {
        return ClassValueType::Invalid;
    }
}

class ClassValue {
public:

    using ValuePtrT = void* ClassValue::*;
    using CreateFuncT = std::function<void(void*, ValuePtrT, const char*)>;

public:

    template<typename ClassT, typename ValueT>
    static ValuePtrT CastToPtr(ValueT ClassT::* valuePtr) {
        static_assert(sizeof(ValueT ClassT::*) == sizeof(ValuePtrT),
            "The size of pointer to class member does not equal to the size of value ptr holder");

        void* voidPtr = static_cast<void*>(&valuePtr);
        ValuePtrT* valPtr = static_cast<ValuePtrT*>(voidPtr);
        return *valPtr;
    }

    template<typename ClassT>
    static void* ClassT::* CastFromPtr(ValuePtrT ptr) {
        using ResultT = void* ClassT::*;
        void* voidPtr = static_cast<void*>(&ptr);
        ResultT* valuePtr = static_cast<ResultT*>(voidPtr);
        ResultT value = *valuePtr;
        return value;
    }

public:

    ClassValue();
    ~ClassValue();

    const char* getTypeName() const;

public:

    std::string name;
    ClassValueType type;
    ValuePtrT ptr;
    TypeId typeId;
    CreateFuncT createFunc;
};

#endif /* __CLASS_VALUE_HPP__ */