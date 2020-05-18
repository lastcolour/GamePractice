#ifndef __REFLECT_UTILS_HPP__
#define __REFLECT_UTILS_HPP__

#include "Core/Core.hpp"
#include "Core/TypeId.hpp"
#include "Math/Vector.hpp"
#include "Render/Color.hpp"

#include <vector>
#include <type_traits>
#include <functional>

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
    Array,
    Entity
};

namespace ReflectUtils {

template<typename>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T,A>> : std::true_type {};

using ReflectFuncT = std::function<void(ReflectContext&)>;

bool ReflectClassByCall(TypeId instanceTypeId, ReflectFuncT reflectFunc);

using ArrayCreateElemFuncT = std::function<void*(void*)>;
using ArraySizeFuncT = std::function<size_t(void*)>;
using ArrayEraseElemFuncT = std::function<void(size_t, void*)>;
using ArrayGetElemFuncT = std::function<void*(size_t, void*)>;

bool RegisterArrayInfo(TypeId elemTypeId, ClassValueType elemType, ArrayCreateElemFuncT createFunc, ArraySizeFuncT sizeFunc,
    ArrayEraseElemFuncT eraseFunc, ArrayGetElemFuncT getElemFunc);

} // namespace ReflectUtils

template<typename ValueT>
constexpr ClassValueType GetClassValueType() {
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
    } else if constexpr (std::is_enum<ValueT>::value) {
        return ClassValueType::Enum;
    } else if constexpr (ReflectUtils::is_std_vector<ValueT>::value) {
        return ClassValueType::Array;
    } else if constexpr (std::is_same<ValueT, EntityId>::value) {
        return ClassValueType::Entity;
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

template<typename ClassT>
bool CreateTypeInfo() {
    constexpr auto type = GetClassValueType<ClassT>();
    static_assert(type != ClassValueType::Invalid, "Can't add field with unknown type");
    if constexpr (type == ClassValueType::Object) {
        auto reflectFunc = [](ReflectContext& ctx){
            ClassT::Reflect(ctx);
        };
        return ReflectUtils::ReflectClassByCall(
            GetTypeId<ClassT>(),
            static_cast<ReflectUtils::ReflectFuncT>(reflectFunc)
        );
    } else if constexpr (type == ClassValueType::Array) {
        using ElemT = typename ClassT::value_type;
        if(!CreateTypeInfo<ElemT>()) {
            return false;
        }
        static_assert(GetClassValueType<ElemT>() != ClassValueType::Array, "Array of arrays is not supported");
        auto createElemFunc = [](void* value) {
            auto arrayPtr = static_cast<ClassT*>(value);
            arrayPtr->emplace_back();
            return &(arrayPtr->back());
        };
        auto sizeFunc = [](void* value) {
            auto arrayPtr = static_cast<ClassT*>(value);
            return arrayPtr->size();
        };
        auto eraseFunc = [](size_t pos, void* value) {
            auto arrayPtr = static_cast<ClassT*>(value);
            arrayPtr->erase(arrayPtr->begin() + pos);
        };
        auto getElemFunc = [](size_t pos, void* value) {
            auto arrayPtr = static_cast<ClassT*>(value);
            return &((*arrayPtr)[pos]);
        };
        return ReflectUtils::RegisterArrayInfo(
            GetTypeId<ElemT>(),
            GetClassValueType<ElemT>(),
            static_cast<ReflectUtils::ArrayCreateElemFuncT>(createElemFunc),
            static_cast<ReflectUtils::ArraySizeFuncT>(sizeFunc),
            static_cast<ReflectUtils::ArrayEraseElemFuncT>(eraseFunc),
            static_cast<ReflectUtils::ArrayGetElemFuncT>(getElemFunc)
        );
    }
    return true;
}

#endif /* __REFLECT_UTILS_HPP__ */