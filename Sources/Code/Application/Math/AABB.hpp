#ifndef __AABB_HPP__
#define __AABB_HPP__

#include "Math/Vector.hpp"

namespace Math {

struct AABB {
public:

    AABB() {}
    AABB(const Vec3& bt, const Vec3& tp) :
        bot(bt), top(tp) {}
    AABB(const AABB& other) :
        bot(other.bot), top(other.top) {}

    Vec3 getSize() const {
        return top - bot;
    }
    Vec3 getCenter() const {
        return (top + bot) * 0.5f;
    }
    void setCenter(const Vec3& pt) {
        Vec3 shift = pt - getCenter();
        bot = bot + shift;
        top = top + shift;
    }

public:

    Vec3 bot;
    Vec3 top;
};

struct AABB2D {
public:

    AABB2D() {}
    AABB2D(const Vec2& bt, const Vec2& tp) :
        bot(bt), top(tp) {}
    AABB2D(const AABB2D& other) :
        bot(other.bot), top(other.top) {}

    Vec2 getSize() const {
        return top - bot;
    }
    Vec2 getCenter() const {
        return (top + bot) * 0.5f;
    }
    void setCenter(const Vec2& pt) {
        Vec2 shift = pt - getCenter();
        bot = bot + shift;
        top = top + shift;
    }

public:

    Vec2 bot;
    Vec2 top;
};

inline AABB operator+(const AABB& aabb, const Vec3& pos) {
    return AABB(aabb.bot + pos, aabb.top + pos);
}

inline AABB2D operator+(const AABB2D& aabb, const Vec2& pos) {
    return AABB2D(aabb.bot + pos, aabb.top + pos);
}

} // namespace Math

typedef Math::AABB AABB;
typedef Math::AABB2D AABB2D;

#endif /* __AABB_HPP__ */