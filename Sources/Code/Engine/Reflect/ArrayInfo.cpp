#include "Reflect/ArrayInfo.hpp"
#include <vector>
#include "Core/JSONNode.hpp"

ArrayInfo::ArrayInfo(TypeId elemTypeId, ClassValueType elemType, CreateElemFuncT createF, SizeFuncT sizeF, EraseElemFuncT eraseF) :
    createFunc(createF),
    sizeFunc(sizeF),
    eraseFunc(eraseF) {

    elemValue.isElement = true;
    elemValue.typeId = elemTypeId;
    elemValue.type = elemType;
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
