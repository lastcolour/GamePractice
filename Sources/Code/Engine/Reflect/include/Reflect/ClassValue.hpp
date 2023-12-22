#ifndef __CLASS_VALUE_HPP__
#define __CLASS_VALUE_HPP__

#include "Reflect/ReflectCore.hpp"

namespace Reflect {

class ClassValue {
public:

    using ValuePtrT = void* ClassValue::*;

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

    std::string getTypeName() const;
    bool writeValueTo(const SerializeContext& cxt, void* valuePtr, Memory::MemoryStream& stream);
    bool writeValueTo(const SerializeContext& cxt, void* valuePtr, JSONNode& node);
    bool readValueFrom(const SerializeContext& cxt, void* valuePtr, Memory::MemoryStream& stream);
    bool readValueFrom(const SerializeContext& cxt, void* valuePtr, const JSONNode& node);
    bool addArrayElement(void* valuePtr);
    bool setPolymorphType(void* valuePtr, const char* typeName);
    void setDefaultValue(void* valuePtr);
    bool isSimple() const;

public:

    std::string name;
    ClassValueType type;
    ValuePtrT ptr;
    Core::TypeId typeId;
    int primitiveValueCount;
    bool isElement;
};

} // namespace Reflect

#endif /* __CLASS_VALUE_HPP__ */