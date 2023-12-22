#ifndef __CLASS_INFO_HPP__
#define __CLASS_INFO_HPP__

#include "Reflect/ClassValue.hpp"
#include "Reflect/ClassInstance.hpp"
#include "Core/ObjectPool.hpp"

namespace Reflect {

class ClassInfo {

    using GetValueFuncT = void*(*)(void*,ClassValue::ValuePtrT);

public:

    ClassInfo(const char* name, Core::TypeId typeId);
    ~ClassInfo();

    ClassInstance createInstance();
    void removeInstance(void* ptr);
    bool reCreateInstance(void* ptr);
    const char* getName() const;
    Core::TypeId getIntanceTypeId() const;
    void makeReflectModel(JSONNode& node);
    bool isDerivedFrom(const ClassInfo& other) const;

    bool writeValueTo(const SerializeContext& ctx, void* instance, ClassValueId valueId, JSONNode& node);
    bool writeValueTo(const SerializeContext& ctx, void* instance, ClassValueId valueId, Memory::MemoryStream& stream);
    bool readValueFrom(const SerializeContext& ctx, void* instance, ClassValueId valueId, const JSONNode& node);
    bool readValueFrom(const SerializeContext& ctx, void* instance, ClassValueId valueId, Memory::MemoryStream& stream);
    bool addNewValueArrayElement(void* instance, ClassValueId valueId);
    bool setValuePolymorphType(void* instance, ClassValueId valueId, const char* newType);

    template<typename ClassT>
    void init() {
        getValueFunc = [](void* object, ClassValue::ValuePtrT ptr) -> void* {
            auto valuePtr = ClassValue::CastFromPtr<ClassT>(ptr);
            auto instance = static_cast<ClassT*>(object);
            return &(instance->*valuePtr);
        };
        if constexpr (!std::is_abstract<ClassT>::value) {
            pool.reset(new Memory::ObjectPool<ClassT>());
        }
    }

    template<typename ClassT, typename ValueT>
    void addField(const char* name, ValueT ClassT::* valuePtr) {
        constexpr auto type = GetClassValueType<ValueT>();
        static_assert(type != ClassValueType::Invalid, "Can't add field with unknown type");
        if(!checkIfSameType(Core::GetTypeId<ClassT>())) {
            return;
        }
        if(!CreateTypeInfo<ValueT>()) {
            return;
        }
        auto valueTypeId = Core::InvalidTypeId;
        if constexpr (type == ClassValueType::Array) {
            valueTypeId = Core::GetTypeId<typename ValueT::value_type>();
        } else if constexpr (type == ClassValueType::Resource) {
            valueTypeId = Core::GetTypeId<typename ValueT::SpecType>();
        } else if constexpr (type == ClassValueType::PolymorphObject){
            valueTypeId = Core::GetTypeId<typename ValueT::ObjectType>();
        } else {
            valueTypeId = Core::GetTypeId<ValueT>();
        }
        registerClassValue(name, type, ClassValue::CastToPtr(valuePtr), valueTypeId);
    }

    template<typename ClassT>
    void addBaseClass() {
        constexpr auto type = GetClassValueType<ClassT>();
        static_assert(type == ClassValueType::Object, "Base class should have object type");
        if(!CreateTypeInfo<ClassT>()) {
            return;
        }
        registerBaseClass(Core::GetTypeId<ClassT>());
    }

private:

    bool checkIfSameType(Core::TypeId typeId) const;
    const ClassValue* findValueByName(const char* name) const;
    const ClassValue* findValueByPtr(ClassValue::ValuePtrT ptr) const;
    ClassValue* findValueById(void*& instance, int valueId);
    ClassValue* findValueByPrimitiveValueId(void*& instance, int valueId);
    void registerBaseClass(Core::TypeId baseClassTypeId);
    void registerClassValue(const char* valueName, ClassValueType valueType, ClassValue::ValuePtrT valuePtr, Core::TypeId valueTypeId);
    void getAllClasses(std::vector<ClassInfo*>& classes);

private:

    std::vector<ClassInfo*> baseClasses;
    std::vector<ClassValue> values;
    std::unique_ptr<Memory::Impl::BaseObjectPool> pool;
    GetValueFuncT getValueFunc;
    std::string className;
    Core::TypeId instanceTypeId;
    int primitiveValueCount;
};

} // namespace Reflect

#endif /* __CLASS_INFO_HPP__ */