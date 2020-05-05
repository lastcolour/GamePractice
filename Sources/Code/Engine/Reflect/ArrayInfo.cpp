#include "Reflect/ArrayInfo.hpp"
#include "Core/JSONNode.hpp"
#include "Core/StringFormat.hpp"

ArrayInfo::ArrayInfo(TypeId elemTypeId, ClassValueType elemType, CreateElemFuncT createF, SizeFuncT sizeF, EraseElemFuncT eraseF) :
    createFunc(createF),
    sizeFunc(sizeF),
    eraseFunc(eraseF) {

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