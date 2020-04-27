#ifndef __CLASS_INFO_HPP__
#define __CLASS_INFO_HPP__

#include "Reflect/ClassValue.hpp"
#include "Reflect/ClassIntance.hpp"

#include <vector>

class JSONNode;
class ReflectContext;

using ClassInstanceDeleteFuncT = std::function<void(void*)>;

class ClassInfo {

public:

    ClassInfo();
    ~ClassInfo();

    ClassInstance createInstance(const JSONNode& node);
    ClassInstanceDeleteFuncT getDeleteFunction() const;
    const char* getName() const;
    TypeId getIntanceTypeId() const;


    template<typename ClassT>
    bool init(const char* name) {
        if(!isClassNameValid(name)) {
            return false;
        }
        className = name;
        instanceTypeId = GetTypeId<ClassT>();
        deleteFunc = [](void* object){
            delete static_cast<ClassT*>(object);
        };
        createFunc = []() -> void* {
            return static_cast<void*>(new ClassT);
        };
        getValueFunc = [](void* object, ClassValue::ValuePtrT ptr) -> void* {
            auto instance = static_cast<ClassT*>(object);
            auto value = reinterpret_cast<void* ClassT::*>(ptr);
            return &(instance->*value);
        };
        return true;
    }

    template<typename ClassT, typename ValueT>
    void addField(const char* name, ValueT ClassT::* value) {
        if(getIntanceTypeId() != GetTypeId<ClassT>()) {
            return;
        }
        if(!value) {
            return;
        }
        if(!isFieldNameValid(name)) {
            return;
        }
        constexpr auto valueType = getClassValueType<ValueT>();
        if constexpr (valueType == ClassValueType::Invalid) {
            return;
        } else if constexpr (valueType == ClassValueType::Object) {
            auto intanceTypeId = GetTypeId<ValueT>();
            if(!findClassInfo(intanceTypeId)) {
                auto reflectFunc = [](ReflectContext& ctx) { 
                    ValueT::reflect(ctx);
                };
                if(!reflectEmbebedClass(reflectFunc)) {
                    return;
                }
            }
        }
        ClassValue classValue;
        classValue.name = name;
        classValue.type = valueType;
        classValue.ptr = reinterpret_cast<ClassValue::ValuePtrT>(value);
        classValue.typeId = GetTypeId<ValueT>();
        values.push_back(classValue);
    }

private:

    bool isClassNameValid(const char* name) const;
    bool isFieldNameValid(const char* name) const;
    bool serializeInstance(void* instance, const JSONNode& node);
    bool reflectEmbebedClass(const std::function<void(ReflectContext& ctx)>& reflectFunc);
    ClassInfo* findClassInfo(TypeId instanceTypeId) const;

private:

    std::function<void*(void)> createFunc;
    std::function<void(void*)> deleteFunc;
    std::function<void*(void*, ClassValue::ValuePtrT)> getValueFunc;
    std::string className;
    TypeId instanceTypeId;
    std::vector<ClassValue> values;
};

#endif /* __CLASS_INFO_HPP__ */