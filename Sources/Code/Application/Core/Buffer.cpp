#include "Buffer.hpp"

Buffer::Buffer() :
    size(0u),
    data() {}

Buffer::Buffer(Buffer&& buff) :
    size(buff.size),
    data(std::move(buff.data)) {

    buff.size = 0u;
}

Buffer::Buffer(size_t buffSize) : size(0u) {
    if(buffSize > 0) {
        resize(buffSize);
    }
}

Buffer& Buffer::operator=(Buffer&& buff) {
    if(this != &buff) {
        data = std::move(buff.data);
        size = buff.size;
        buff.size = 0u;
    }
    return *this;
}

Buffer::operator bool() const {
    return size != 0u;
}

std::string Buffer::getString() const {
    if(size == 0) {
        return "";
    }
    auto cStr = reinterpret_cast<const char*>(data.get());
    auto cStrLength = strlen(cStr);
    return std::string(cStr, cStrLength);
}

size_t Buffer::getSize() const {
    if(size) {
        return size-1;
    }
    return 0u;
}

void* Buffer::getData() {
    return static_cast<void*>(data.get());
}

void Buffer::resize(size_t newSize) {
    if(newSize == 0u) {
        size = 0u;
        data.reset();
        return;
    }
    size_t actualNewSize = newSize + 1;
    if(actualNewSize < size) {
        return;
    }
    uint8_t* newData = new uint8_t[actualNewSize];
    if(!newData) {
        data.reset();
    } else {
        newData[actualNewSize-1] = 0u;
        if(!size) {
            newData[0] = 0u;
        }
        memcpy(newData, data.get(), size);
        size = actualNewSize;
        data.reset(newData);
    }
}