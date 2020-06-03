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

    ClassInstance createInstance(const JSONNode& node);
    ClassInstance createDefaultInstance();
    bool serializeInstance(void* instance, const JSONNode& node);
    DeleteFuncT getDeleteFunction() const;
    const char* getName() const;
    TypeId getIntanceTypeId() const;
    void makeReflectModel(JSONNode& node);

    bool readValue(void* instance, EntityLogicValueId valueId, MemoryStream& stream);
    bool writeValue(void* instance, EntityLogicValueId valueId, MemoryStream& stream);

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
        if(getIntanceTypeId() != GetTypeId<ClassT>()) {
            return;
        }
        if(!CreateTypeInfo<ValueT>()) {
            return;
        }
        auto valueTypeId = InvalidTypeId;
        if constexpr (type == ClassValueType::Array) {
            using ElemT = typename ValueT::value_type;
            valueTypeId = GetTypeId<ElemT>();
        } else {
            valueTypeId = GetTypeId<ValueT>();
        }
        registerClassValue(name, type, ClassValue::CastToPtr(valuePtr), valueTypeId, nullptr);
    }

    template<typename ClassT>
    void addResourceField(const char* name, void (ClassT::*setFunc)(const char*)) {
        if(getIntanceTypeId() != GetTypeId<ClassT>()) {
            return;
        }
        ClassValue::SetResourceFuncT valueSetFunc = [setFunc](void* instance, const char* resourceName){
            (static_cast<ClassT*>(instance)->*setFunc)(resourceName);
        };
        registerClassValue(name, ClassValueType::Resource, nullptr, InvalidTypeId, valueSetFunc);
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

    const ClassValue* findValueByName(const char* name) const;
    const ClassValue* findValueByPtr(ClassValue::ValuePtrT ptr) const;
    ClassValue* findValueById(int valueId);
    void registerBaseClass(TypeId baseClassTypeId);
    void registerClassValue(const char* valueName, ClassValueType valueType, ClassValue::ValuePtrT valuePtr, TypeId valueTypeId,
        ClassValue::SetResourceFuncT valueSetFunc);
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