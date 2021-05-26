#ifndef __CLASS_INFO_HPP__
#define __CLASS_INFO_HPP__

#include "Reflect/ClassValue.hpp"
#include "Reflect/ClassInstance.hpp"

class ClassInfo {

    using DeleteFuncT = void(*)(void*);
    using CreateFuncT = void*(*)(void);
    using GetValueFuncT = void*(*)(void*,ClassValue::ValuePtrT);

public:

    ClassInfo(const char* name, TypeId typeId);
    ~ClassInfo();

    ClassInstance createInstance();
    DeleteFuncT getDeleteFunction() const;
    const char* getName() const;
    TypeId getIntanceTypeId() const;
    void makeReflectModel(JSONNode& node);
    bool isDerivedFrom(const ClassInfo& other) const;

    bool writeValueTo(const SerializeContext& ctx, void* instance, EntityLogicValueId valueId, JSONNode& node);
    bool writeValueTo(const SerializeContext& ctx, void* instance, EntityLogicValueId valueId, MemoryStream& stream);
    bool readValueFrom(const SerializeContext& ctx, void* instance, EntityLogicValueId valueId, const JSONNode& node);
    bool readValueFrom(const SerializeContext& ctx, void* instance, EntityLogicValueId valueId, MemoryStream& stream);
    bool addNewValueArrayElement(void* instance, EntityLogicValueId valueId);
    bool setValuePolymorphType(void* instance, EntityLogicValueId valueId, const char* newType);

    template<typename ClassT>
    void init() {
        getValueFunc = [](void* object, ClassValue::ValuePtrT ptr) -> void* {
            auto valuePtr = ClassValue::CastFromPtr<ClassT>(ptr);
            auto instance = static_cast<ClassT*>(object);
            return &(instance->*valuePtr);
        };
        if constexpr (!std::is_abstract<ClassT>::value) {
            createFunc = []() -> void* {
                return static_cast<void*>(new ClassT);
            };
            deleteFunc = [](void* object){
                delete static_cast<ClassT*>(object);
            };
        }
    }

    template<typename ClassT, typename ValueT>
    void addField(const char* name, ValueT ClassT::* valuePtr) {
        constexpr auto type = GetClassValueType<ValueT>();
        static_assert(type != ClassValueType::Invalid, "Can't add field with unknown type");
        if(!checkIfSameType(GetTypeId<ClassT>())) {
            return;
        }
        if(!CreateTypeInfo<ValueT>()) {
            return;
        }
        auto valueTypeId = InvalidTypeId;
        if constexpr (type == ClassValueType::Array) {
            valueTypeId = GetTypeId<ValueT::value_type>();
        } else if constexpr (type == ClassValueType::PolymorphObject){
            valueTypeId = GetTypeId<ValueT::ObjectType>();
        } else {
            valueTypeId = GetTypeId<ValueT>();
        }
        registerClassValue(name, type, ClassValue::CastToPtr(valuePtr), valueTypeId, ResourceType::Invalid, nullptr);
    }

    template<typename ClassT>
    void addResourceField(const char* name, ResourceType resType, void (ClassT::*setFunc)(const char*)) {
        if(!checkIfSameType(GetTypeId<ClassT>())) {
            return;
        }
        ClassValue::SetResourceFuncT valueSetFunc = [setFunc](void* instance, const char* resourceName){
            (static_cast<ClassT*>(instance)->*setFunc)(resourceName);
        };
        registerClassValue(name, ClassValueType::Resource, nullptr, InvalidTypeId, resType, valueSetFunc);
    }

    template<typename ClassT>
    void addResourceField(const char* name, ResourceType resType, std::string ClassT::* valuePtr) {
        if(!checkIfSameType(GetTypeId<ClassT>())) {
            return;
        }
        registerClassValue(name, ClassValueType::Resource, ClassValue::CastToPtr(valuePtr), InvalidTypeId, resType, nullptr);
    }

    template<typename ClassT>
    void addBaseClass() {
        constexpr auto type = GetClassValueType<ClassT>();
        static_assert(type == ClassValueType::Object, "Base class should have object type");
        if(!CreateTypeInfo<ClassT>()) {
            return;
        }
        registerBaseClass(GetTypeId<ClassT>());
    }

private:

    bool checkIfSameType(TypeId typeId) const;
    const ClassValue* findValueByName(const char* name) const;
    const ClassValue* findValueByPtr(ClassValue::ValuePtrT ptr) const;
    ClassValue* findValueById(void*& instance, int valueId);
    ClassValue* findValueByPrimitiveValueId(void*& instance, int valueId);
    void registerBaseClass(TypeId baseClassTypeId);
    void registerClassValue(const char* valueName, ClassValueType valueType, ClassValue::ValuePtrT valuePtr, TypeId valueTypeId,
        ResourceType resType, ClassValue::SetResourceFuncT valueSetFunc);
    void getAllClasses(std::vector<ClassInfo*>& classes);

private:

    std::vector<ClassInfo*> baseClasses;
    std::vector<ClassValue> values;
    CreateFuncT createFunc;
    DeleteFuncT deleteFunc;
    GetValueFuncT getValueFunc;
    std::string className;
    TypeId instanceTypeId;
    int primitiveValueCount;
};

#endif /* __CLASS_INFO_HPP__ */