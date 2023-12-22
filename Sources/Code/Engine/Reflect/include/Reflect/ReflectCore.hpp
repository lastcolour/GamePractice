#ifndef __REFLECT_CORE_HPP__
#define __REFLECT_CORE_HPP__

#include "Math/Color.hpp"
#include "Math/Vector.hpp"
#include "Math/Quaternion.hpp"

class JSONNode;
class ReflectContext;

namespace Reflect {

class ClassValue;
class ClassInfo;
class EnumInfo;
class ArrayInfo;
class ResourceInfo;

template<typename T>
class Resource;

struct SerializeContext {
    EntityId entityId;
};

using ClassValueId = int;
const ClassValueId InvalidClassValueId = -1;
const ClassValueId AllClassValuesId = 0;

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
    Entity,
    PolymorphObject
};

template<typename T>
class PolymorphPtr;

namespace Impl {

class BaseResource;

template<typename>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T,A>> : std::true_type {};

template<typename>
struct is_polymorh_ptr : std::false_type {};

template<typename T>
struct is_polymorh_ptr<PolymorphPtr<T>> : std::true_type {};

template<typename T, typename F = void>
struct is_resource : std::false_type {};

template<typename T>
struct is_resource<T, std::enable_if_t<std::is_base_of<BaseResource, T>::value>> : std::true_type {};

template<typename T>
class ResourceSpecTypeCheck {

    template<typename>
    static std::false_type test_to_storage_func(...);

    template<typename U>
    static auto test_to_storage_func(int) ->
        decltype(std::declval<U>().Convert(std::declval<const typename U::RuntimeType&>(), std::declval<typename U::StorageType&>()),
            std::true_type());

    template<typename>
    static std::false_type test_to_runtime_func(...);

    template<typename U>
    static auto test_to_runtime_func(int) ->
        decltype(std::declval<U>().Convert(std::declval<const typename U::StorageType&>(), std::declval<typename U::RuntimeType&>()),
            std::true_type());

    template<typename>
    static std::false_type test_storage_type(...);

    template<typename U>
    static auto test_storage_type(int)
        -> decltype(std::declval<typename U::StorageType>(),
            std::true_type());

    template<typename>
    static std::false_type test_runtime_type(...);

    template<typename U>
    static auto test_runtime_type(int)
        -> decltype(std::declval<typename U::RuntimeType>(),
            std::true_type());

public:

    static const size_t MaxStorageTypeSize = Resource<T>::MaxStorageTypeSize;

public:

    static constexpr bool has_storage_type = std::is_same<decltype(test_storage_type<T>(0)), std::true_type>::value;
    static constexpr bool has_runtime_type = std::is_same<decltype(test_runtime_type<T>(0)), std::true_type>::value;

    static constexpr bool has_to_storage_func = std::is_same<decltype(test_to_storage_func<T>(0)), std::true_type>::value;
    static constexpr bool has_to_runtime_func = std::is_same<decltype(test_to_runtime_func<T>(0)), std::true_type>::value;
};

template<typename T, typename F = void>
struct has_reflect_func : std::false_type {};

template<typename T>
struct has_reflect_func<T, std::enable_if_t<std::is_function<decltype(T::Reflect)>::value>> : std::true_type {};

using ReflectFuncT = std::function<void(ReflectContext&)>;

bool ReflectClassByCall(Core::TypeId instanceTypeId, ReflectFuncT reflectFunc);

using ArrayCreateElemFuncT = std::function<void*(void*)>;
using ArraySizeFuncT = std::function<size_t(void*)>;
using ArrayGetElemFuncT = std::function<void*(size_t, void*)>;
using ArrayResetFuncT = std::function<void(void*)>;

bool RegisterArrayInfo(Core::TypeId elemTypeId, ClassValueType elemType, ArrayCreateElemFuncT createFunc,
    ArraySizeFuncT sizeFunc, ArrayGetElemFuncT getElemFunc, ArrayResetFuncT resetFunc);

using ResourceConvertFuncT = std::function<void(const void*, void*)>;
using ResourceTempBuffInitFuncT = std::function<void(void*, bool)>;

bool RegisterResourceInfo(Core::TypeId resourceTypeId, ClassValueType elemType, Core::TypeId elemTypeId, const char* name,
    ResourceConvertFuncT toRuntimeConvertFunc, ResourceConvertFuncT toStoreConvertFunc, ResourceTempBuffInitFuncT tempBufferInitFunc);

template<typename T>
constexpr bool IsReflectable() {
    if constexpr (has_reflect_func<T>::value) {
        return std::is_same<decltype(&T::Reflect), void(*)(ReflectContext&)>::value;
    }
    return false;
}

} // namespace Impl

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
    } else if constexpr (Impl::is_std_vector<ValueT>::value) {
        return ClassValueType::Array;
    } else if constexpr (std::is_same<ValueT, EntityId>::value) {
        return ClassValueType::Entity;
    } else if constexpr (Impl::is_polymorh_ptr<ValueT>::value) {
        static_assert(Impl::IsReflectable<typename ValueT::ObjectType>(), "Object type isn't reflectable");
        return ClassValueType::PolymorphObject;
    } else if constexpr (Impl::is_resource<ValueT>::value) {
        return ClassValueType::Resource;
    } else if constexpr (Impl::IsReflectable<ValueT>()) {
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
        return Impl::ReflectClassByCall(
            Core::GetTypeId<ClassT>(),
            static_cast<Impl::ReflectFuncT>(reflectFunc)
        );
    } else if constexpr (type == ClassValueType::PolymorphObject) {
        return CreateTypeInfo<typename ClassT::ObjectType>();
    } else if constexpr (type == ClassValueType::Resource) {

        using SpecT = typename ClassT::SpecType;
        using SpecTypeCheck = Impl::ResourceSpecTypeCheck<SpecT>;

        static_assert(SpecTypeCheck::has_storage_type, "StorageType is not specified");
        static_assert(SpecTypeCheck::has_runtime_type, "Runtime is not specified");

        using StorageT = typename SpecT::StorageType;
        using RuntimeT = typename SpecT::RuntimeType;

        static_assert(GetClassValueType<StorageT>() != ClassValueType::Resource, "Resource of resource is not supported");

        if(!CreateTypeInfo<StorageT>()) {
            return false;
        }

        static_assert(sizeof(StorageT) < SpecTypeCheck::MaxStorageTypeSize, "StorageType is too big");

        const char* ResourceName = SpecT().TypeName;
        if constexpr (!std::is_same<StorageT, RuntimeT>::value) {
            static_assert(SpecTypeCheck::has_to_storage_func, "Convert func from RuntimeType to StorageType is not specified");
            static_assert(SpecTypeCheck::has_to_runtime_func, "Convert func from StorageType to RuntimeType is not specified");

            auto toRuntime = [](const void* from, void* to) {
                auto input = static_cast<const StorageT*>(from);
                auto ouput = static_cast<RuntimeT*>(to);
                SpecT::Convert(*input, *ouput);
            };
            auto toStorage = [](const void* from, void* to) {
                auto input = static_cast<const RuntimeT*>(from);
                auto ouput = static_cast<StorageT*>(to);
                SpecT::Convert(*input, *ouput);
            };
            auto tempBuffInit = [](void* ptr, bool doInit){
                if(doInit) {
                    new (ptr) StorageT();
                } else {
                    static_cast<StorageT*>(ptr)->~StorageT();
                }
            };
            return Impl::RegisterResourceInfo(Core::GetTypeId<SpecT>(),
                GetClassValueType<StorageT>(),
                Core::GetTypeId<StorageT>(),
                ResourceName,
                static_cast<Impl::ResourceConvertFuncT>(toRuntime),
                static_cast<Impl::ResourceConvertFuncT>(toStorage),
                static_cast<Impl::ResourceTempBuffInitFuncT>(tempBuffInit));
        } else {
            return Impl::RegisterResourceInfo(Core::GetTypeId<SpecT>(),
                GetClassValueType<StorageT>(),
                Core::GetTypeId<StorageT>(),
                ResourceName,
                nullptr,
                nullptr,
                nullptr);
        }
    } else if constexpr (type == ClassValueType::Array) {
        using ElemT = typename ClassT::value_type;
        static_assert(GetClassValueType<ElemT>() != ClassValueType::Array, "Array of arrays is not supported");
        if(!CreateTypeInfo<ElemT>()) {
            return false;
        }
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
        return Impl::RegisterArrayInfo(
            Core::GetTypeId<ElemT>(),
            GetClassValueType<ElemT>(),
            static_cast<Impl::ArrayCreateElemFuncT>(createElemFunc),
            static_cast<Impl::ArraySizeFuncT>(sizeFunc),
            static_cast<Impl::ArrayGetElemFuncT>(getElemFunc),
            static_cast<Impl::ArrayResetFuncT>(resetFunc)
        );
    } else {
        return true;
    }
}

} // namespace Reflect

#endif /* __REFLECT_CORE_HPP__ */