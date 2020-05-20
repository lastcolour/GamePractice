#ifndef __ARRAY_INFO_HPP__
#define __ARRAY_INFO_HPP__

#include "Reflect/ClassValue.hpp"

class ArrayInfo {
public:

    using CreateElemFuncT = ReflectUtils::ArrayCreateElemFuncT;
    using SizeFuncT = ReflectUtils::ArraySizeFuncT;
    using EraseElemFuncT = ReflectUtils::ArrayEraseElemFuncT;
    using GetElemFuncT = ReflectUtils::ArrayGetElemFuncT;

public:

    ArrayInfo(TypeId elemTypeId, ClassValueType elemType, CreateElemFuncT createF, SizeFuncT sizeF,
        EraseElemFuncT eraseF, GetElemFuncT getElemF);
    ~ArrayInfo();

    void* createElement(void* valuePtr);
    size_t size(void* valuePtr) const;
    void eraseElement(size_t pos, void* valuePtr);
    bool serializeElement(void* element, const JSONNode& node);
    TypeId getElemTypeId() const;
    const char* getName() const;
    void makeReflectModel(JSONNode& node);
    bool readValues(void* valuePtr, MemoryStream& stream);
    bool writeValues(void* valuePtr, MemoryStream& stream);

private:

    std::string name;
    ClassValue elemValue;
    CreateElemFuncT createFunc;
    SizeFuncT sizeFunc;
    EraseElemFuncT eraseFunc;
    GetElemFuncT getElemFunc;
};

#endif /* __ARRAY_INFO_HPP__ */