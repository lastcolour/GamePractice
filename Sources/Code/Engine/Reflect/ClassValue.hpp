#ifndef __CLASS_VALUE_HPP__
#define __CLASS_VALUE_HPP__

#include "Reflect/ReflectUtils.hpp"

class JSONNode;
class MemoryStream;

class ClassValue {
public:

    using ValuePtrT = void* ClassValue::*;
    using SetResourceFuncT = std::function<void(void*, const char*)>;

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
    bool readValue(void* instance, void* valuePtr, const JSONNode& node);
    bool readValue(void* instance, void* valuePtr, MemoryStream& stream) const;
    bool writeValue(void* instance, void* valuePtr, MemoryStream& stream);
    bool addArrayElement(void* valuePtr);
    void setDefaultValue(void* valuePtr);

public:

    std::string name;
    ClassValueType type;
    ValuePtrT ptr;
    TypeId typeId;
    SetResourceFuncT setResourceFunc;
    int primitiveValueCount;
    bool isElement;
};

#endif /* __CLASS_VALUE_HPP__ */