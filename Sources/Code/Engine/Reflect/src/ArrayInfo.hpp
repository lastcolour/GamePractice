#ifndef __ARRAY_INFO_HPP__
#define __ARRAY_INFO_HPP__

#include "Reflect/ClassValue.hpp"

class ArrayInfo {
public:

    using CreateElemFuncT = ReflectCore::ArrayCreateElemFuncT;
    using SizeFuncT = ReflectCore::ArraySizeFuncT;
    using GetElemFuncT = ReflectCore::ArrayGetElemFuncT;
    using ResetFuncT = ReflectCore::ArrayResetFuncT;

public:

    ArrayInfo(TypeId elemTypeId, ClassValueType elemType, CreateElemFuncT createF, SizeFuncT sizeF,
        GetElemFuncT getElemF, ResetFuncT resetF);
    ~ArrayInfo();

    TypeId getElemTypeId() const;
    const char* getName() const;
    bool writeValuesTo(const SerializeContext& ctx, void* valuePtr, JSONNode& node);
    bool writeValuesTo(const SerializeContext& ctx, void* valuePtr, MemoryStream& stream);
    bool readValuesFrom(const SerializeContext& ctx, void* valuePtr, const JSONNode& node);
    bool readValuesFrom(const SerializeContext& ctx, void* valuePtr, MemoryStream& stream);
    void setDefaultValue(void* valuePtr);
    bool addElement(void* valuePtr);

private:

    std::string name;
    ClassValue elemValue;
    CreateElemFuncT createFunc;
    SizeFuncT sizeFunc;
    GetElemFuncT getElemFunc;
    ResetFuncT resetFunc;
};

#endif /* __ARRAY_INFO_HPP__ */