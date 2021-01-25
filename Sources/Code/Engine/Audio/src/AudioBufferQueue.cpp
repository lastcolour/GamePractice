#include "AudioBufferQueue.hpp"

#include <cassert>

AudioBuffer::AudioBuffer() :
    data(nullptr),
    offset(0),
    size(0),
    readSize(0) {
}

AudioBuffer::AudioBuffer(AudioBuffer&& other) :
    data(std::move(other.data)),
    offset(other.offset),
    size(other.size),
    readSize(other.readSize) {

    other.offset = 0;
    other.size = 0;
    other.readSize = 0;
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) {
    if(this != &other) {
        data = std::move(other.data);
        offset = other.offset;
        size = other.size;
        readSize = other.readSize;

        other.offset = 0;
        other.size = 0;
        other.readSize = 0;
    }
    return *this;
}

AudioBuffer::~AudioBuffer() {
}

void AudioBuffer::resize(int newSize) {
    assert(newSize > 0 && "Invalid size");
    if(newSize != size) {
        data.reset(new char[newSize]);
    }
    size = newSize;
    offset = 0;
    readSize = 0;
}

void AudioBuffer::setReadSize(int newReadSize) {
    readSize = newReadSize;
}

void* AudioBuffer::getPtr() {
    char* src = data.get();
    return src + offset;
}

int AudioBuffer::getAvaibleSizeForRead() const {
    return readSize - offset;
}

int AudioBuffer::getTotalSize() const {
    return size;
}

void AudioBuffer::setReadDone(int newSize) {
    offset += newSize;
}

AudioBufferQueue::AudioBufferQueue() {
}

AudioBufferQueue::~AudioBufferQueue() {
}

void AudioBufferQueue::init(int numBuffers) {
    assert(buffers.empty() && "Audio queue already initialized");
    assert(numBuffers > 0 && "Invalid numBuffres");

    buffers.resize(numBuffers);
    for(int i = 0; i < numBuffers; ++i) {
        pendingWrite.push_back(i);
    }
}

AudioBuffer* AudioBufferQueue::getNextRead() {
    std::lock_guard<std::mutex> lock(mutex);
    if(pendingRead.empty()) {
        return nullptr;
    }
    int id = pendingRead.front();
    pendingRead.erase(pendingRead.begin());
    return &(buffers[id]);
}

void AudioBufferQueue::putReadDone(AudioBuffer* buffer) {
    std::lock_guard<std::mutex> lock(mutex);
    int buffId = -1;
    for(int i = 0, sz = buffers.size(); i < sz; ++i) {
        if(&(buffers[i]) == buffer) {
            buffId = i;
            break;
        }
    }
    assert(buffId != -1 && "Can't find buffer");
    if(buffer->getAvaibleSizeForRead() > 0) {
        pendingRead.insert(pendingRead.begin(), buffId);
    } else {
        pendingWrite.push_back(buffId);
    }
}

std::vector<AudioBuffer*> AudioBufferQueue::getNextWrites() {
    std::vector<AudioBuffer*> res;
    {
        std::lock_guard<std::mutex> lock(mutex);
        for(auto& buffId : pendingWrite) {
            res.push_back(&(buffers[buffId]));
        }
        pendingWrite.clear();
    }
    return res;
}

void AudioBufferQueue::putWritesDone(std::vector<AudioBuffer*>& writeBuffers) {
    std::lock_guard<std::mutex> lock(mutex);
    for(auto& writeBuffer : writeBuffers) {
        assert(writeBuffer->getAvaibleSizeForRead() > 0 && "Empty read size");
        assert(writeBuffer->getAvaibleSizeForRead() <= writeBuffer->getTotalSize() && "Read read size greater total size");
        int buffId = -1;
        for(int i = 0, sz = buffers.size(); i < sz; ++i) {
            if(&(buffers[i]) == writeBuffer) {
                buffId = i;
                break;
            }
        }
        assert(buffId != -1 && "Can't find buffer");
        pendingRead.push_back(buffId);
    }
}