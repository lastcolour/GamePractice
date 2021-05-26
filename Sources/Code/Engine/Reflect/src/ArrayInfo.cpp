#include "ArrayInfo.hpp"
#include "Core/JSONNode.hpp"
#include "Core/MemoryStream.hpp"

#include <cassert>

ArrayInfo::ArrayInfo(TypeId elemTypeId, ClassValueType elemType, CreateElemFuncT createF,
    SizeFuncT sizeF, GetElemFuncT getElemF, ResetFuncT resetF) :
    createFunc(createF),
    sizeFunc(sizeF),
    getElemFunc(getElemF),
    resetFunc(resetF) {

    elemValue.isElement = true;
    elemValue.typeId = elemTypeId;
    elemValue.type = elemType;

    name = StringFormat("array.%s", elemValue.getTypeName());
}

ArrayInfo::~ArrayInfo() {
}

TypeId ArrayInfo::getElemTypeId() const {
    return elemValue.typeId;
}

const char* ArrayInfo::getName() const {
    return name.c_str();
}

bool ArrayInfo::readValuesFrom(const SerializeContext& ctx, void* valuePtr, const JSONNode& node) {
    resetFunc(valuePtr);
    for(auto& elemNode : node) {
        auto elem = createFunc(valuePtr);
        if(!elemValue.readValueFrom(ctx, elem, elem, elemNode)) {
            return false;
        }
    }
    return true;
}

bool ArrayInfo::readValuesFrom(const SerializeContext& ctx, void* valuePtr, MemoryStream& stream) {
    resetFunc(valuePtr);
    int sz = 0;
    stream.read(sz);
    for(int i = 0; i < sz; ++i) {
        auto elem = createFunc(valuePtr);
        if(!elemValue.readValueFrom(ctx, elem, elem, stream)) {
            return false;
        }
    }
    return true;
}

bool ArrayInfo::writeValuesTo(const SerializeContext& ctx, void* valuePtr, MemoryStream& stream) {
    int sz = static_cast<int>(sizeFunc(valuePtr));
    stream.write(sz);
    for(int i = 0; i < sz; ++i) {
        auto elem = getElemFunc(i, valuePtr);
        if(!elemValue.writeValueTo(ctx, elem, elem, stream)) {
            return false;
        }
    }
    return true;
}

bool ArrayInfo::writeValuesTo(const SerializeContext& ctx, void* valuePtr, JSONNode& node) {
    int sz = static_cast<int>(sizeFunc(valuePtr));
    auto setRes = node.setArray();
    assert(setRes && "Can't set node to array");
    for(int i = 0; i < sz; ++i) {
        auto elem = getElemFunc(i, valuePtr);
        if(!elemValue.writeValueTo(ctx, elem, elem, node)) {
            return false;
        }
    }
    return true;
}

bool ArrayInfo::addElement(void* valuePtr) {
    auto elem = createFunc(valuePtr);
    if(!elem) {
        elemValue.setDefaultValue(elem);
    }
    return elem != nullptr;
}

void ArrayInfo::setDefaultValue(void* valuePtr) {
    resetFunc(valuePtr);
}