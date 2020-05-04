#ifndef __CLASS_INFO_HPP__
#define __CLASS_INFO_HPP__

#include "Reflect/ClassValue.hpp"
#include "Reflect/ClassIntance.hpp"
#include "Reflect/EnumInfo.hpp"

#include <vector>

class JSONNode;
class ReflectContext;

using ClassInstanceDeleteFuncT = std::function<void(void*)>;

class ClassInfo {

    using CreateFuncT = std::function<void*(void)>;
    using GetValueFuncT = std::function<void*(void*, ClassValue::ValuePtrT)>;
    using ReflectFuncT = std::function<void(ReflectContext& ctx)>;

public:

    ClassInfo(const char* name, TypeId typeId);
    ~ClassInfo();

    ClassInstance createInstance(const JSONNode& node);
    bool serializeInstance(void* instance, const JSONNode& node);
    ClassInstanceDeleteFuncT getDeleteFunction() const;
    const char* getName() const;
    TypeId getIntanceTypeId() const;
    void makeReflectModel(JSONNode& node);

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
        constexpr auto type = getClassValueType<ValueT>();
        static_assert(type != ClassValueType::Invalid, "Can't add field with unknown type");
        if(getIntanceTypeId() != GetTypeId<ClassT>()) {
            return;
        }
        if constexpr (type == ClassValueType::Object) {
            auto intanceTypeId = GetTypeId<ValueT>();
            if(!findClassInfo(intanceTypeId)) {
                auto reflectFunc = [](ReflectContext& ctx) {
                    ValueT::Reflect(ctx);
                };
                if(!reflectEmbebedClass(reflectFunc)) {
                    return;
                }
            }
        } else if constexpr (type == ClassValueType::Array) {
            return;
        }
        registerClassValue(name, type, ClassValue::CastToPtr(valuePtr), GetTypeId<ValueT>(), nullptr);
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
        auto classInfo = findClassInfo(GetTypeId<ClassT>());
        if(!classInfo) {
            ReflectContext ctx;
            ctx.reflect<ClassT>();
            classInfo = ctx.getRegisteredClassInfo();
        }
        registerBaseClass(classInfo);
    }

private:

    
    ClassValue* findValueByName(const char* name);
    ClassValue* findValueByPtr(ClassValue::ValuePtrT ptr);
    bool reflectEmbebedClass(ReflectFuncT reflectFunc);
    ClassInfo* findClassInfo(TypeId instanceTypeId) const;
    void registerBaseClass(ClassInfo* baseClassInfo);
    void registerClassValue(const char* valueName, ClassValueType valueType, ClassValue::ValuePtrT valuePtr, TypeId valueTypeId,
        ClassValue::SetResourceFuncT valueSetFunc);
    void getAllClasses(std::vector<ClassInfo*> classes);

private:

    std::vector<ClassInfo*> baseClasses;
    std::vector<ClassValue> values;
    CreateFuncT createFunc;
    ClassInstanceDeleteFuncT deleteFunc;
    GetValueFuncT getValueFunc;
    std::string className;
    TypeId instanceTypeId;
};

#endif /* __CLASS_INFO_HPP__ */