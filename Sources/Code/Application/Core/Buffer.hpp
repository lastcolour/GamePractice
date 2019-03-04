#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__

#include <memory>
#include <string>

class Buffer {
public:

    Buffer();

    Buffer(Buffer&& buff);
    explicit Buffer(size_t buffSize);

    ~Buffer() = default;

    Buffer& operator=(Buffer&& buff);
    explicit operator bool() const;

    std::string getString() const;

    size_t getSize() const;
    void* getData();
    void resize(size_t newSize);

private:

    size_t size;
    std::unique_ptr<uint8_t[]> data;
};

#endif /* __BUFFER_HPP__ */