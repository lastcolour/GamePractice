#ifndef __MEMORY_STREAM_HPP__
#define __MEMORY_STREAM_HPP__

class MemoryStream {
public:

    MemoryStream();
    ~MemoryStream();

    void openForRead(Buffer& buffer);
    void openForWrite();

    bool isOpenedForRead() const;
    bool isOpenedForWrite() const;
    bool isClosed() const;

    void reopenForRead();
    void reopenForWrite();

    Buffer flushToBuffer();
    void close();
    size_t size() const;
    size_t tellg() const;

    void write(bool val);
    void write(char val);
    void write(unsigned char val);
    void write(float val);
    void write(int val);
    void write(const char* val);

    void read(bool& val);
    void read(char& val);
    void read(unsigned char& val);
    void read(float& val);
    void read(int& val);
    void read(std::string& val);

private:

    enum class StreamState {
        Closed = 0,
        Write,
        Read
    };

private:

    void grow(size_t amount);
    void* getWritePtr();
    const void* getReadPtr() const;

private:

    Buffer buffer;
    size_t pos;
    StreamState state;
};

#endif /* __MEMORY_STREAM_HPP__ */