#include "Core/MemoryStream.hpp"

#include <cassert>

namespace {

const size_t BUFFER_START_SIZE = 32;

template<typename T>
T& getRef(void* ptr) {
    return *(static_cast<T*>(ptr));
}

template<typename T>
const T& getRef(const void* ptr) {
    return *(static_cast<const T*>(ptr));
}

} // namespace

MemoryStream::MemoryStream() :
    buffer(),
    pos(0u),
    state(StreamState::Closed) {
}

MemoryStream::~MemoryStream() {
}

bool MemoryStream::isOpenedForRead() const {
    return state == StreamState::Read;
}

bool MemoryStream::isOpenedForWrite() const {
    return state == StreamState::Write;
}

bool MemoryStream::isClosed() const {
    return state == StreamState::Closed;
}

void MemoryStream::openForRead(Buffer& buff) {
    if(state != StreamState::Closed) {
        assert(false && "Can't open stream that isn't closed");
        return;
    }
    state = StreamState::Read;
    buffer = buff;
}

void MemoryStream::openForWrite() {
    if(state != StreamState::Closed) {
        assert(false && "Can't open stream that isn't closed");
        return;
    }
    state = StreamState::Write;
}

void MemoryStream::reopenForRead() {
    if(state != StreamState::Write) {
        assert(false && "Can't reopen for read stream that is not opened for write");
        return;
    }
    state = StreamState::Read;
    pos = 0;
}

void MemoryStream::reopenForWrite() {
    if(state != StreamState::Read) {
        assert(false && "Can't reopen for write stream that is not opened for read");
        return;
    }
    state = StreamState::Write;
    pos = 0;
    buffer.resize(0);
}

Buffer MemoryStream::flushToBuffer() {
    if(state != StreamState::Write) {
        assert(false && "Can't flush non-write buffer");
        return Buffer();
    }
    Buffer resBuffer;
    resBuffer.resize(pos);
    memcpy(resBuffer.getWriteData(), buffer.getReadData(), pos);
    buffer.resize(0);
    pos = 0;
    return resBuffer;
}

void MemoryStream::close() {
    buffer.resize(0);
    pos = 0;
    state = StreamState::Closed;
}

void MemoryStream::grow(size_t amount) {
    auto buffSize = buffer.getSize();
    if(buffer.getSize() == 0) {
        buffer.resize(BUFFER_START_SIZE);
    } else if(buffSize < pos + amount) {
        buffer.resize(buffSize * 2);
    }
}

void* MemoryStream::getWritePtr() {
    if(buffer.getSize() <= pos) {
        assert(false && "Trying to write out of memory");
        return nullptr;
    }
    auto resPtr = (static_cast<char*>(buffer.getWriteData()) + pos);
    return static_cast<void*>(resPtr);
}

const void* MemoryStream::getReadPtr() const {
    if(buffer.getSize() <= pos) {
        assert(false && "Trying to read out of memory");
        return nullptr;
    }
    auto resPtr = (static_cast<const char*>(buffer.getReadData()) + pos);
    return static_cast<const void*>(resPtr);
}

void MemoryStream::write(bool val) {
    if(state != StreamState::Write) {
        assert(false && "Can't write to non-write stream");
        return;
    }
    grow(1);
    auto ptr = getWritePtr();
    getRef<bool>(ptr) = val;
    pos += 1;
}

void MemoryStream::write(char val) {
    if(state != StreamState::Write) {
        assert(false && "Can't write to non-write stream");
        return;
    }
    grow(1);
    auto ptr = getWritePtr();
    getRef<char>(ptr) = val;
    pos += 1;
}

void MemoryStream::write(unsigned char val) {
    if(state != StreamState::Write) {
        assert(false && "Can't write to non-write stream");
        return;
    }
    grow(1);
    auto ptr = getWritePtr();
    getRef<unsigned char>(ptr) = val;
    pos += 1;
}

void MemoryStream::write(float val) {
    if(state != StreamState::Write) {
        assert(false && "Can't write to non-write stream");
        return;
    }
    grow(4);
    auto ptr = getWritePtr();
    getRef<float>(ptr) = val;
    pos += 4;
}

void MemoryStream::write(int val) {
    if(state != StreamState::Write) {
        assert(false && "Can't write to non-write stream");
        return;
    }
    grow(4);
    auto ptr = getWritePtr();
    getRef<int>(ptr) = val;
    pos += 4;
}

void MemoryStream::write(const char* val) {
    if(state != StreamState::Write) {
        assert(false && "Can't write to non-write stream");
        return;
    }
    auto sLen = strlen(val);
    grow(sLen + 1);
    auto ptr = getWritePtr();
    memcpy(ptr, val, sLen);
    static_cast<char*>(ptr)[sLen] = '\0';
    pos += sLen + 1;
}

void MemoryStream::read(bool& val) {
    if(state != StreamState::Read) {
        assert(false && "Can't read from non-read stream");
        return;
    }
    auto ptr = getReadPtr();
    val = getRef<bool>(ptr);
    pos += 1;
}

void MemoryStream::read(char& val) {
    if(state != StreamState::Read) {
        assert(false && "Can't read from non-read stream");
        return;
    }
    auto ptr = getReadPtr();
    val = getRef<char>(ptr);
    pos += 1;
}

void MemoryStream::read(unsigned char& val) {
    if(state != StreamState::Read) {
        assert(false && "Can't read from non-read stream");
        return;
    }
    auto ptr = getReadPtr();
    val = getRef<unsigned char>(ptr);
    pos += 1;
}

void MemoryStream::read(float& val) {
    if(state != StreamState::Read) {
        assert(false && "Can't read from non-read stream");
        return;
    }
    auto ptr = getReadPtr();
    val = getRef<float>(ptr);
    pos += 4;
}

void MemoryStream::read(int& val) {
    if(state != StreamState::Read) {
        assert(false && "Can't read from non-read stream");
        return;
    }
    auto ptr = getReadPtr();
    val = getRef<int>(ptr);
    pos += 4;
}

void MemoryStream::read(std::string& val) {
    if(state != StreamState::Read) {
        assert(false && "Can't read from non-read stream");
        return;
    }
    auto ptr = getReadPtr();
    val = static_cast<const char*>(ptr);
    pos += val.size() + 1;
}

size_t MemoryStream::size() const {
    if(state == StreamState::Closed) {
        return 0;
    } else if(state == StreamState::Write) {
        return pos;
    }
    return buffer.getSize();
}

size_t MemoryStream::tellg() const {
    return pos;
}