#ifndef __IMAGE_BUFFER_HPP__
#define __IMAGE_BUFFER_HPP__

class ImageBuffer {
public:

    ImageBuffer();
    ~ImageBuffer();

    Vec2i getSize() const;
    void setSizeAndClear(const Vec2i& newSize);
    ColorB getColor(int w, int h) const;
    Memory::Buffer& getData();
    void clear();

private:

    Vec2i size;
    Memory::Buffer data;
};

#endif /* __IMAGE_BUFFER_HPP__ */