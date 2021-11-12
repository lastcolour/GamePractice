#include "Render/ImageBuffer.hpp"

#include <cassert>

ImageBuffer::ImageBuffer() :
    size(0) {
}

ImageBuffer::~ImageBuffer() {
}

Vec2i ImageBuffer::getSize() const {
    return size;
}

void ImageBuffer::setSizeAndClear(const Vec2i& newSize) {
    if(data.getSize() < static_cast<size_t>(newSize.x * newSize.y * 4)) {
        data.resize(newSize.x * newSize.y * 4);
    }
    size = newSize;
    clear();
}

Memory::Buffer& ImageBuffer::getData() {
    return data;
}

void ImageBuffer::clear() {
    ColorB* colorData = static_cast<ColorB*>(data.getWriteData());
    for(int i=0; i < size.x * size.y; ++i) {
        colorData[i] = ColorB(0, 0, 0);
    }
}

ColorB ImageBuffer::getColor(int w, int h) const {
    assert((w < size.x && h < size.y) && "Invalid access to pixel");
    auto offset = size.x * h + w;
    const ColorB* colorData = static_cast<const ColorB*>(data.getReadData());
    return colorData[offset];
}