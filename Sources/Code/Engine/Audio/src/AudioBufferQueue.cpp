#include "AudioBufferQueue.hpp"

#include <cassert>
#include <cstring>

AudioBuffer::AudioBuffer() :
    data(nullptr),
    offset(0),
    size(0) {
}

AudioBuffer::AudioBuffer(AudioBuffer&& other) :
    data(std::move(other.data)),
    offset(other.offset),
    size(other.size) {

    other.offset = 0;
    other.size = 0;
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) {
    if(this != &other) {
        data = std::move(other.data);
        offset = other.offset;
        size = other.size;

        other.offset = 0;
        other.size = 0;
    }
    return *this;
}

AudioBuffer::~AudioBuffer() {
}

void AudioBuffer::write(void* from, int bytesCount) {
    assert(bytesCount > 0 && "Invalid size");
    if(bytesCount > size) {
        data.reset(new char[bytesCount]);
    }
    memcpy(data.get(), from, bytesCount);
    size = bytesCount;
    offset = 0;
}

int AudioBuffer::read(void* to, int bytesCount) {
    assert(bytesCount >= 0 && "Invalid read size");
    bytesCount = std::min(size - offset, bytesCount);
    memcpy(to, data.get() + offset, bytesCount);
    offset += bytesCount;
    return bytesCount;
}

int AudioBuffer::getReadSize() const {
    return size - offset;
}

bool AudioBuffer::isDone() const {
    assert(offset <= size && "Invalid offset");
    return size == offset;
}

AudioBufferQueue::AudioBufferQueue() {
}

AudioBufferQueue::~AudioBufferQueue() {
}

void AudioBufferQueue::init(int numBuffers) {
    assert(buffers.empty() && "Audio queue already initialized");
    assert(numBuffers > 0 && "Invalid numBuffres");

    buffers.resize(numBuffers);
    writeQueue.init(numBuffers);
    readQueue.init(numBuffers);
    for(int i = 0; i < numBuffers; ++i) {
        writeQueue.push(&buffers[i]);
    }
}

AudioBuffer* AudioBufferQueue::peekRead() {
    auto ptr = readQueue.peek();
    return static_cast<AudioBuffer*>(ptr);
}

void AudioBufferQueue::tryPopRead() {
    auto buff = peekRead();
    assert(buff && "Invalid buffer");
    if(buff->isDone()) {
        readQueue.pop();
        writeQueue.push(buff);
    }
}

std::vector<AudioBuffer*> AudioBufferQueue::peekWrites() {
    std::vector<AudioBuffer*> res;
    auto ptr = writeQueue.peek();
    while(ptr) {
        AudioBuffer* buff = static_cast<AudioBuffer*>(ptr);
        assert(buff->isDone() && "Buffer should be finised");
        res.push_back(buff);
        writeQueue.pop();
        ptr = writeQueue.peek();
    }
    return res;
}

void AudioBufferQueue::submitWrites(std::vector<AudioBuffer*>& writes) {
    for(auto ptr : writes) {
        readQueue.push(ptr);
    }
}