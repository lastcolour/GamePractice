#include "Reflect/ArrayInfo.hpp"
#include "Core/JSONNode.hpp"
#include "Core/StringFormat.hpp"
#include "Core/MemoryStream.hpp"

ArrayInfo::ArrayInfo(TypeId elemTypeId, ClassValueType elemType, CreateElemFuncT createF,
    SizeFuncT sizeF, EraseElemFuncT eraseF, GetElemFuncT getElemF) :
    createFunc(createF),
    sizeFunc(sizeF),
    eraseFunc(eraseF),
    getElemFunc(getElemF) {

    elemValue.isElement = true;
    elemValue.typeId = elemTypeId;
    elemValue.type = elemType;

    name = StringFormat("Array<%s>", elemValue.getTypeName());
}

ArrayInfo::~ArrayInfo() {
}

void* ArrayInfo::createElement(void* valuePtr) {
    return createFunc(valuePtr);
}

size_t ArrayInfo::size(void* valuePtr) const {
    return sizeFunc(valuePtr);
}

void ArrayInfo::eraseElement(size_t pos, void* valuePtr) {
    eraseFunc(pos, valuePtr);
}

bool ArrayInfo::serializeElement(void* element, const JSONNode& node) {
    return elemValue.serializeValue(element, element, node);
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

bool ArrayInfo::dumpValues(void* valuePtr, MemoryStream& stream) {
    int sz = static_cast<int>(size(valuePtr));
    stream.write(sz);
    for(int i = 0; i < sz; ++i) {
        auto elem = getElemFunc(i, valuePtr);
        if(!elemValue.dumpValue(elem, elem, stream)) {
            return false;
        }
    }
    return true;
}