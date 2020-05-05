#ifndef __ARRAY_INFO_HPP__
#define __ARRAY_INFO_HPP__

#include "Reflect/ClassValue.hpp"

class ArrayInfo {
public:

    using CreateElemFuncT = ReflectUtils::ArrayCreateElemFuncT;
    using SizeFuncT = ReflectUtils::ArraySizeFuncT;
    using EraseElemFuncT = ReflectUtils::ArrayEraseElemFuncT;

public:

    ArrayInfo(TypeId elemTypeId, ClassValueType elemType, CreateElemFuncT createF, SizeFuncT sizeF, EraseElemFuncT eraseF);
    ~ArrayInfo();

    void* createElement(void* valuePtr);
    size_t size(void* valuePtr) const;
    void eraseElement(size_t pos, void* valuePtr);
    bool serializeElement(void* element, const JSONNode& node);
    TypeId getElemTypeId() const;

private:

    ClassValue elemValue;
    CreateElemFuncT createFunc;
    SizeFuncT sizeFunc;
    EraseElemFuncT eraseFunc;
};

#endif /* __ARRAY_INFO_HPP__ */