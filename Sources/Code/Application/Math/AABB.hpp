#ifndef __AABB_HPP__
#define __AABB_HPP__

#include "Math/Vector.hpp"

namespace Math {

template<typename T>
struct AABB {
public:

    AABB() {}
    AABB(const T& bt, const T& tp) :
        bot(bt), top(tp) {}
    AABB(const AABB& other) :
        bot(other.bot), top(other.top) {}

    T getSize() const {
        return top - bot;
    }
    T getCenter() const {
        return (top + bot) * 0.5f;
    }
    void setCenter(const T& pt) {
        T shift = pt - getCenter();
        bot = bot + shift;
        top = top + shift;
    }

public:

    T bot;
    T top;
};

template<typename T>
AABB<T> operator+(const AABB<T>& aabb, const T& pos) {
    return AABB<T>(aabb.bot + pos, aabb.top + pos);
}

} // namespace Math

typedef Math::AABB<Vec2> AABB2D;
typedef Math::AABB<Vec2i> AABB2Di;
typedef Math::AABB<Vec3> AABB;

#endif /* __AABB_HPP__ */