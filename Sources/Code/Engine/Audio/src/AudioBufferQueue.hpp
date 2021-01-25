#ifndef __AUDIO_BUFFER_QUEUE_HPP__
#define __AUDIO_BUFFER_QUEUE_HPP__

#include <vector>
#include <memory>
#include <mutex>

class AudioBuffer {
public:

    AudioBuffer();
    AudioBuffer(AudioBuffer&& other);
    AudioBuffer& operator=(AudioBuffer&& other);
    ~AudioBuffer();

    void resize(int newSize);
    void setReadSize(int newReadSize);

    void* getPtr();
    int getAvaibleSizeForRead() const;
    int getTotalSize() const;
    void setReadDone(int newSize);

private:

    AudioBuffer(const AudioBuffer&) = delete;
    AudioBuffer& operator=(const AudioBuffer&) = delete;

private:

    std::unique_ptr<char[]> data;
    int offset;
    int size;
    int readSize;
};

class AudioBufferQueue {
public:

    AudioBufferQueue();
    ~AudioBufferQueue();

    void init(int numBuffers);

    AudioBuffer* getNextRead();
    void putReadDone(AudioBuffer* buffer);

    std::vector<AudioBuffer*> getNextWrites();
    void putWritesDone(std::vector<AudioBuffer*>& writeBuffers);

private:

    std::mutex mutex;
    std::vector<AudioBuffer> buffers;
    std::vector<int> pendingRead;
    std::vector<int> pendingWrite;
};

#endif /* __AUDIO_BUFFER_QUEUE_HPP__ */