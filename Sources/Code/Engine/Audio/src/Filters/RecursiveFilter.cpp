#include "Filters/RecursiveFilter.hpp"
#include "Math/Primitivies.hpp"

#include <cmath>

RecursiveFilter::RecursiveFilter() :
    x1(0.f),
    x2(0.f),
    y1(0.f),
    y2(0.f) {
}

float RecursiveFilter::apply(float val) {
    float y = a0 * val + a1 * x1 + a2 * x2 -
        b1 * y1 - b2 * y2;
    x2 = x1;
    x1 = val;
    y2 = y1;
    y1 = y;
    return y * c0 + val * d0;
}

RecursiveFilter CreateLowPass(float freq) {
    RecursiveFilter filter;

    float x = 2.f * Math::PI * freq;
    float y = cos(x) / (1.f + sin(x));

    filter.a0 = (1.f - y) / 2.f;
    filter.a1 = filter.a0;
    filter.a2 = 0.f;
    filter.b1 = -y;
    filter.b2 = 0.f;
    filter.c0 = 1.f;
    filter.d0 = 0.f;

    return filter;
}

RecursiveFilter CreateHightPass(float freq) {
    RecursiveFilter filter;

    float x = 2.f * Math::PI * freq;
    float y = cos(x) / (1.f + sin(x));

    filter.a0 = (1.f + y) / 2.f;
    filter.a1 = -filter.a0;
    filter.a2 = 0.f;
    filter.b1 = -y;
    filter.b2 = 0.f;
    filter.c0 = 1.f;
    filter.d0 = 0.f;

    return filter;
}

RecursiveFilter CreateLowShelf(float gain, float freq) {
    float u = gain;
    float w = 2.f * Math::PI * freq;
    float v = 4.f / (1.f + u);
    float x = v * tan(w / 2.f);
    float y = (1.f - x) / (1.f + x);

    RecursiveFilter filter;

    filter.a0 = (1.f - y) / 2.f;
    filter.a1 = filter.a0;
    filter.a2 = 0.f;
    filter.b1 = -y;
    filter.b2 = 0.f;
    filter.c0 = u - 1.f;
    filter.d0 = 1.f;

    return filter;
}

RecursiveFilter CreatePeaking(float gain, float q, float freq) {
    float u = gain;
    float v = 4.f / (1.f + u);
    float w = 2.f * Math::PI * freq;
    float x = tan(w / (2.f * q));
    float vx = v * x;
    float y = 0.5f * ((1.f - vx) / (1.f + vx));
    float z = (0.5f + y) * cos(w);

    RecursiveFilter filter;
    filter.a0 = 0.5f - y;
    filter.a1 = 0.f;
    filter.a2 = -filter.a0;
    filter.b1 = -2.f * z;
    filter.b2 = 2.f * y;
    filter.c0 = u - 1.f;
    filter.d0 = 1.f;

    return filter;
}

RecursiveFilter CreateHighShelf(float gain, float freq) {
    float u = gain;
    float w = 2.f * Math::PI * freq;
    float v = (1.f + u) / 4.f;
    float x = v * tan(w / 2.f);
    float y = (1.f - x) / (1.f + x);

    RecursiveFilter filter;

    filter.a0 = (1.f + y) / 2.f;
    filter.a1 = -filter.a0;
    filter.a2 = 0.f;
    filter.b1 = -y;
    filter.b2 = 0.f;
    filter.c0 = u - 1.f;
    filter.d0 = 1.f;

    return filter;
}