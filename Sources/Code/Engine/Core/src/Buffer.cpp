#include <cassert>

struct BufferImpl {
    size_t size;
    std::unique_ptr<uint8_t[]> data;

    BufferImpl() : size(0u), data() {}
    BufferImpl(size_t buffSize) : size(buffSize) {
        if(buffSize > 0) {
            data.reset(new uint8_t[buffSize]);
            if(!data) {
                assert(false && "Can't allocate memory");
            } else {
                data[0] = 0u;
            }
        }
    }
    BufferImpl(BufferImpl&& buff) : size(buff.size), data(std::move(buff.data)) {
        buff.size = 0u;
    }

    void reset() {
        size = 0u;
        data.reset();
    }
};

Buffer::Buffer() :
    buffImpl(nullptr) {}

Buffer::Buffer(const Buffer& buff) :
    buffImpl(buff.buffImpl) {
}

Buffer::Buffer(Buffer&& buff) :
    buffImpl(std::move(buff.buffImpl)) {
}

Buffer::Buffer(size_t buffSize) :
    buffImpl() {
    resize(buffSize);
}

Buffer::Buffer(const void* dataPtr, size_t dataSize) :
    buffImpl() {
    resize(dataSize);
    if(dataSize > 0) {
        assert(dataPtr != nullptr && "Invalid source data");
        memcpy(buffImpl->data.get(), dataPtr, dataSize);
    }
}

Buffer::~Buffer() {
}

Buffer& Buffer::operator=(const Buffer& buff) {
    if(this != &buff) {
        buffImpl = buff.buffImpl;
    }
    return *this;
}

Buffer& Buffer::operator=(Buffer&& buff) {
    if(this != &buff) {
        buffImpl = std::move(buff.buffImpl);
    }
    return *this;
}

Buffer::operator bool() const {
    return buffImpl != nullptr && buffImpl->size > 0u;
}

std::string Buffer::acquireString() {
    if(buffImpl == nullptr || buffImpl->size == 0u) {
        return "";
    }
    buffImpl->data[buffImpl->size - 1] = 0;
    auto cStr = reinterpret_cast<const char*>(buffImpl->data.get());
    auto cStrLength = strlen(cStr);
    std::string resStr(cStr, cStrLength);
    buffImpl->reset();
    return resStr;
}

std::string_view Buffer::getString() const {
    if(buffImpl == nullptr || buffImpl->size == 0u) {
        return "";
    }
    buffImpl->data[buffImpl->size - 1] = 0;
    auto cStr = reinterpret_cast<const char*>(buffImpl->data.get());
    auto cStrLength = strlen(cStr);
    return std::string_view(cStr, cStrLength);
}

const char* Buffer::getCString() const {
    if(buffImpl == nullptr || buffImpl->size == 0u) {
        return "";
    }
    buffImpl->data[buffImpl->size - 1] = 0;
    return reinterpret_cast<const char*>(buffImpl->data.get());
}

size_t Buffer::getSize() const {
    if(buffImpl != nullptr && buffImpl->size > 0u) {
        return buffImpl->size - 1;
    }
    return 0u;
}

const void* Buffer::getReadData() const {
    if(buffImpl != nullptr) {
        return static_cast<const void*>(buffImpl->data.get());
    }
    return nullptr;
}

void* Buffer::getWriteData() {
    if(buffImpl != nullptr) {
        if(buffImpl.use_count() > 1u) {
            auto prevBuffer = buffImpl;
            buffImpl.reset(new BufferImpl(prevBuffer->size));
            memcpy(buffImpl->data.get(), prevBuffer->data.get(), prevBuffer->size);
        }
        return static_cast<void*>(buffImpl->data.get());
    }
    return nullptr;
}

void Buffer::resize(size_t newSize) {
    if(newSize == 0u) {
        buffImpl.reset();
        return;
    }
    size_t actualNewSize = newSize + 1;
    if(buffImpl != nullptr && buffImpl->size >= actualNewSize) {
        return;
    }
    auto prevBuffer = buffImpl;
    buffImpl.reset(new BufferImpl(actualNewSize));
    if(!buffImpl->size) {
        buffImpl.reset();
    } else {
        buffImpl->data[actualNewSize-1] = 0u;
        if(prevBuffer != nullptr && prevBuffer->size > 0) {
            memcpy(buffImpl->data.get(), prevBuffer->data.get(), prevBuffer->size);
        } else {
            buffImpl->data[0] = 0u;
        }
    }
}

void Buffer::reset() {
    buffImpl.reset();
}