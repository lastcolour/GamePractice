#include "Reflect/ArrayInfo.hpp"
#include "Core/JSONNode.hpp"
#include "Core/StringFormat.hpp"
#include "Core/MemoryStream.hpp"

ArrayInfo::ArrayInfo(TypeId elemTypeId, ClassValueType elemType, CreateElemFuncT createF,
    SizeFuncT sizeF, GetElemFuncT getElemF, ResetFuncT resetF) :
    createFunc(createF),
    sizeFunc(sizeF),
    getElemFunc(getElemF),
    resetFunc(resetF) {

    elemValue.isElement = true;
    elemValue.typeId = elemTypeId;
    elemValue.type = elemType;

    name = StringFormat("Array<%s>", elemValue.getTypeName());
}

ArrayInfo::~ArrayInfo() {
}

TypeId ArrayInfo::getElemTypeId() const {
    return elemValue.typeId;
}

const char* ArrayInfo::getName() const {
    return name.c_str();
}

void ArrayInfo::makeReflectModel(JSONNode& node) {
    node.write("type", "array");
    JSONNode elemNode;
    elemNode.write("type", elemValue.getTypeName());
    node.write("data", elemNode);
}

bool ArrayInfo::readValues(void* valuePtr, const JSONNode& node) {
    resetFunc(valuePtr);
    for(auto& elemNode : node) {
        auto elem = createFunc(valuePtr);
        if(!elemValue.readValue(elem, elem, elemNode)) {
            return false;
        }
    }
    return true;
}

bool ArrayInfo::readValues(void* valuePtr, MemoryStream& stream) {
    int sz = static_cast<int>(sizeFunc(valuePtr));
    stream.write(sz);
    for(int i = 0; i < sz; ++i) {
        auto elem = getElemFunc(i, valuePtr);
        if(!elemValue.readValue(elem, elem, stream)) {
            return false;
        }
    }
    return true;
}

bool ArrayInfo::writeValues(void* valuePtr, MemoryStream& stream) {
    resetFunc(valuePtr);
    int sz = 0;
    stream.read(sz);
    for(int i = 0; i < sz; ++i) {
        auto elem = createFunc(valuePtr);
        if(!elemValue.writeValue(elem, elem, stream)) {
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