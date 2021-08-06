#ifndef __COLOR_HPP__
#define __COLOR_HPP__

#include <stdint.h>
#include <cmath>

namespace Color {

    static const float PIXEL_EQUAL_TOLERANCE = 0.01f;

    struct Color_RGBA_Float {

        explicit Color_RGBA_Float(float val) :
            r(val), g(val), b(val), a(1.f) {}
        Color_RGBA_Float(float red, float green, float blue) :
            r(red), g(green), b(blue), a(1.f) {}
        Color_RGBA_Float(float red, float green, float blue, float alpha) :
            r(red), g(green), b(blue), a(alpha) {}
        Color_RGBA_Float(const Color_RGBA_Float& other) :
            r(other.r), g(other.g), b(other.b), a(other.a) {}
        Color_RGBA_Float() = default;

        Color_RGBA_Float& operator=(const Color_RGBA_Float& col) {
            r = col.r;
            g = col.g;
            b = col.b;
            a = col.a;
            return *this;
        }

        bool operator==(const Color_RGBA_Float& col) const {
            return std::abs(r - col.r) < PIXEL_EQUAL_TOLERANCE
                && std::abs(g - col.g) < PIXEL_EQUAL_TOLERANCE
                && std::abs(b - col.b) < PIXEL_EQUAL_TOLERANCE;
        }

        bool operator!=(const Color_RGBA_Float& col) const {
            return !(*this == col);
        }

        const float* getPtr() const {
            return &r;
        }

        float r;
        float g;
        float b;
        float a;
    };

    struct Color_RGBA_Byte {

        explicit Color_RGBA_Byte(uint8_t val) :
            r(val), g(val), b(val), a(255) {}
        Color_RGBA_Byte(uint8_t red, uint8_t green, uint8_t blue) :
            r(red), g(green), b(blue), a(255) {}
        Color_RGBA_Byte(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) :
            r(red), g(green), b(blue), a(alpha) {}
        Color_RGBA_Byte(const Color_RGBA_Byte& other) :
            r(other.r), g(other.g), b(other.b), a(other.a) {}
        Color_RGBA_Byte() = default;

        Color_RGBA_Byte& operator=(const Color_RGBA_Byte& col) {
            r = col.r;
            g = col.g;
            b = col.b;
            a = col.a;
            return *this;
        }

        bool operator==(const Color_RGBA_Byte& col) const {
            return r == col.r && g == col.g && b == col.b && a == col.a;
        }

        bool operator!=(const Color_RGBA_Byte& col) const {
            return !(*this == col);
        }

        const uint8_t* getPtr() const {
            return &r;
        }

        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

inline Color_RGBA_Byte toColorB(const Color_RGBA_Float& col) {
    return Color_RGBA_Byte(
        static_cast<uint8_t>(col.r * 255),
        static_cast<uint8_t>(col.g * 255),
        static_cast<uint8_t>(col.b * 255),
        static_cast<uint8_t>(col.a * 255));
}

inline Color_RGBA_Float toColorF(const Color_RGBA_Byte& col) {
    return Color_RGBA_Float(
        static_cast<float>(col.r / 255.f),
        static_cast<float>(col.g / 255.f),
        static_cast<float>(col.b / 255.f),
        static_cast<float>(col.a / 255.f));
}

} // namespace Color

using ColorF = Color::Color_RGBA_Float;
using ColorB = Color::Color_RGBA_Byte;

#endif /* __COLOR_HPP__ */