#ifndef __COLOR_HPP__
#define __COLOR_HPP__

#include <stdint.h>
#include <cmath>

namespace Color {

    static const float PIXEL_EQUAL_TOLERANCE = 0.01f;

    struct Color_RGBA_Float {

        Color_RGBA_Float(float red, float green, float blue) : 
            r(red), g(green), b(blue), a(1.f) {}
            
        Color_RGBA_Float(float red, float green, float blue, float alpha) : 
            r(red), g(green), b(blue), a(alpha) {}

        Color_RGBA_Float() {}

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

        Color_RGBA_Byte(uint8_t red, uint8_t green, uint8_t blue) :
            r(red), g(green), b(blue), a(255) {}
            
        Color_RGBA_Byte(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) :
            r(red), g(green), b(blue), a(alpha) {}

        Color_RGBA_Byte() {}

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

} // namespace Color

typedef Color::Color_RGBA_Float ColorF;

#endif /* __COLOR_HPP__ */