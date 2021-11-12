#ifndef __ARRAY_INFO_HPP__
#define __ARRAY_INFO_HPP__

namespace Reflect {

class ArrayInfo {
public:

    using CreateElemFuncT = Impl::ArrayCreateElemFuncT;
    using SizeFuncT = Impl::ArraySizeFuncT;
    using GetElemFuncT = Impl::ArrayGetElemFuncT;
    using ResetFuncT = Impl::ArrayResetFuncT;

public:

    ArrayInfo(Core::TypeId elemTypeId, ClassValueType elemType, CreateElemFuncT createF, SizeFuncT sizeF,
        GetElemFuncT getElemF, ResetFuncT resetF);
    ~ArrayInfo();

    Core::TypeId getElemTypeId() const;
    const char* getName() const;
    bool writeValuesTo(const SerializeContext& ctx, void* valuePtr, JSONNode& node);
    bool writeValuesTo(const SerializeContext& ctx, void* valuePtr, Memory::MemoryStream& stream);
    bool readValuesFrom(const SerializeContext& ctx, void* valuePtr, const JSONNode& node);
    bool readValuesFrom(const SerializeContext& ctx, void* valuePtr, Memory::MemoryStream& stream);
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

} // namespace Reflect

#endif /* __ARRAY_INFO_HPP__ */