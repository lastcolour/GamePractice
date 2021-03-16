#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__

struct BufferImpl;

class Buffer {
public:

    Buffer();
    Buffer(const Buffer& buff);
    Buffer(Buffer&& buff);
    Buffer(const void* dataPtr, size_t dataSize);
    explicit Buffer(size_t buffSize);

    ~Buffer();

    Buffer& operator=(Buffer&& buff);
    Buffer& operator=(const Buffer& buff);
    explicit operator bool() const;

    std::string acquireString();
    std::string_view getString() const;
    const char* getCString() const;

    size_t getSize() const;
    void* getWriteData();
    const void* getReadData() const;
    void resize(size_t newSize);
    void reset();

private:

    std::shared_ptr<BufferImpl> buffImpl;
};

#endif /* __BUFFER_HPP__ */