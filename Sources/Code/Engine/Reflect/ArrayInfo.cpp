#include "Reflect/ArrayInfo.hpp"
#include <vector>
#include "Core/JSONNode.hpp"

ArrayInfo::ArrayInfo() {
}

ArrayInfo::~ArrayInfo() {
}

void* ArrayInfo::createElement(void* valuePtr) {
    return createElemFunc(valuePtr);
}
