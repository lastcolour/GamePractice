#ifndef __REFLECT_CORE_HPP__
#define __REFLECT_CORE_HPP__

#include "Core/Core.hpp"
#include "Core/TypeId.hpp"
#include "Math/Vector.hpp"
#include "Math/Quaternion.hpp"
#include "Render/Color.hpp"

#include <vector>
#include <type_traits>
#include <functional>
#include <string>

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
    Quat,
    Color,
    Object,
    Resource,
    Enum,
    Array,
    Entity
};

enum class ResourceType {
    Invalid = 0,
    Entity,
    Sound,
    SoundEvent,
    Image
};

namespace ReflectCore {

template<typename>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T,A>> : std::true_type {};

using ReflectFuncT = std::function<void(ReflectContext&)>;

bool ReflectClassByCall(TypeId instanceTypeId, ReflectFuncT reflectFunc);

using ArrayCreateElemFuncT = std::function<void*(void*)>;
using ArraySizeFuncT = std::function<size_t(void*)>;
using ArrayGetElemFuncT = std::function<void*(size_t, void*)>;
using ArrayResetFuncT = std::function<void(void*)>;

bool RegisterArrayInfo(TypeId elemTypeId, ClassValueType elemType, ArrayCreateElemFuncT createFunc, ArraySizeFuncT sizeFunc,
    ArrayGetElemFuncT getElemFunc, ArrayResetFuncT resetFunc);

} // namespace ReflectCore

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
    } else if constexpr (std::is_same<ValueT, Quat>::value) {
        return ClassValueType::Quat;
    } else if constexpr (std::is_same<ValueT, ColorB>::value) {
        return ClassValueType::Color;
    } else if constexpr (std::is_enum<ValueT>::value) {
        static_assert(sizeof(ValueT) == sizeof(int), "Invalid enum size");
        return ClassValueType::Enum;
    } else if constexpr (ReflectCore::is_std_vector<ValueT>::value) {
        return ClassValueType::Array;
    } else if constexpr (std::is_same<ValueT, EntityId>::value) {
        return ClassValueType::Entity;
    } else if constexpr (std::is_function<decltype(ValueT::Reflect)>::value) {
        static_assert(std::is_same<decltype(&ValueT::Reflect), void(*)(ReflectContext&)>::value,
            "Invalid object type");
        return ClassValueType::Object;
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
        return ReflectCore::ReflectClassByCall(
            GetTypeId<ClassT>(),
            static_cast<ReflectCore::ReflectFuncT>(reflectFunc)
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
        auto getElemFunc = [](size_t pos, void* value) {
            auto arrayPtr = static_cast<ClassT*>(value);
            return &((*arrayPtr)[pos]);
        };
        auto resetFunc = [](void* value) {
            auto arrayPtr = static_cast<ClassT*>(value);
            arrayPtr->clear();
        };
        return ReflectCore::RegisterArrayInfo(
            GetTypeId<ElemT>(),
            GetClassValueType<ElemT>(),
            static_cast<ReflectCore::ArrayCreateElemFuncT>(createElemFunc),
            static_cast<ReflectCore::ArraySizeFuncT>(sizeFunc),
            static_cast<ReflectCore::ArrayGetElemFuncT>(getElemFunc),
            static_cast<ReflectCore::ArrayResetFuncT>(resetFunc)
        );
    }
    return true;
}

#endif /* __REFLECT_CORE_HPP__ */