#ifndef __CLASS_INFO_HPP__
#define __CLASS_INFO_HPP__

#include "Reflect/ClassValue.hpp"
#include "Reflect/ClassIntance.hpp"

#include <vector>

class JSONNode;
class ReflectContext;

using ClassInstanceDeleteFuncT = std::function<void(void*)>;

class ClassInfo {

    using CreateFuncT = std::function<void*(void)>;
    using DeleteFuncT = std::function<void(void*)>;
    using GetValueFuncT = std::function<void*(void*, ClassValue::ValuePtrT)>;
    using ReflectFuncT = std::function<void(ReflectContext& ctx)>;

public:

    ClassInfo();
    ~ClassInfo();

    ClassInstance createInstance(const JSONNode& node);
    ClassInstanceDeleteFuncT getDeleteFunction() const;
    const char* getName() const;
    TypeId getIntanceTypeId() const;
    void makeReflectModel(JSONNode& node);

    template<typename ClassT>
    bool init(const char* name) {
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
            return initClassInfo(name, GetTypeId<ClassT>(), createFunc, deleteFunc, getValueFunc);
        } else {
            return initClassInfo(name, GetTypeId<ClassT>(), nullptr, nullptr, getValueFunc);
        }
    }

    template<typename ClassT, typename ValueT>
    void addField(const char* name, ValueT ClassT::* valuePtr) {
        static_assert(!std::is_enum<ValueT>::value, "Enums should be added via 'addFieldEnum'");
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

    template<typename ClassT, typename ValueT, typename CreateFuncT>
    void addResourceField(const char* name, ValueT ClassT::* valuePtr, CreateFuncT createResourceFunc) {
        if(getIntanceTypeId() != GetTypeId<ClassT>()) {
            return;
        }
        auto createFunc = [createResourceFunc, this](void* instance, ClassValue::ValuePtrT ptr, const char* resourceName) {
            auto valuePtr = getValueFunc(instance, ptr);
            *(static_cast<ValueT*>(valuePtr)) = createResourceFunc(resourceName);
        };
        registerClassValue(name, ClassValueType::Resource, ClassValue::CastToPtr(valuePtr), GetTypeId<ValueT>(), createFunc);
    }

    template<typename ClassT, typename ValueT>
    void addEnumField(const char* name, ValueT ClassT::* valuePtr) {
        static_assert(std::is_enum<ValueT>::value, "Only enums can be added via this method");
        if(getIntanceTypeId() != GetTypeId<ClassT>()) {
            return;
        }
        registerClassValue(name, ClassValueType::Enum, ClassValue::CastToPtr(valuePtr), GetTypeId<ValueT>(), nullptr);
    }

    template<typename ClassT>
    void addBaseClass() {
        ReflectContext ctx;
        if(!ctx.reflect<ClassT>()) {
            return;
        }
        registerBaseClass(ctx);
    }

private:

    bool serializeInstance(void* instance, const JSONNode& node);
    bool reflectEmbebedClass(ReflectFuncT reflectFunc);
    ClassInfo* findClassInfo(TypeId instanceTypeId) const;
    void registerBaseClass(ReflectContext& ctx);
    bool initClassInfo(const char* name, TypeId objectTypeId, CreateFuncT createF, DeleteFuncT deleteF, GetValueFuncT getValueF);
    void registerClassValue(const char* valueName, ClassValueType valueType, ClassValue::ValuePtrT valuePtr, TypeId valueTypeId,
        ClassValue::CreateFuncT valueCreateFunc);

private:

    std::vector<TypeId> baseClasses;
    std::vector<ClassValue> values;
    CreateFuncT createFunc;
    DeleteFuncT deleteFunc;
    GetValueFuncT getValueFunc;
    std::string className;
    TypeId instanceTypeId;
};

#endif /* __CLASS_INFO_HPP__ */