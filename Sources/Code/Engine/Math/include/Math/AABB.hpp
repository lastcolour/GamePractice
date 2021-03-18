#ifndef __AABB_HPP__
#define __AABB_HPP__

namespace Math {

template<typename T>
struct AABB {
public:

    AABB() = default;
    AABB(const T& bt, const T& tp) :
        bot(bt), top(tp) {}
    AABB(const AABB& other) :
        bot(other.bot), top(other.top) {}
    template<typename F>
    explicit AABB(const F& val) :
        bot(val), top(val) {}

    AABB& operator=(const AABB& other) {
        bot = other.bot;
        top = other.top;
        return *this;
    }

    bool isInside(const T& pos) const {
        return pos >= bot && pos <= top;
    }

    T getSize() const {
        return top - bot;
    }

    T getCenter() const {
        return (top + bot) / static_cast<typename T::ValueType>(2);
    }

    void setCenter(const T& pt) {
        T shift = pt - getCenter();
        bot += shift;
        top += shift;
    }

public:

    T bot;
    T top;
};

template<typename T>
AABB<T> operator+(const AABB<T>& aabb, const T& pos) {
    return AABB<T>(aabb.bot + pos, aabb.top + pos);
}

template<typename T>
bool operator==(const AABB<T>& first, const AABB<T>& second) {
    return first.top == second.top && first.bot == second.bot;
}

template<typename T>
bool operator!=(const AABB<T>& first, const AABB<T>& second) {
    return !(first == second);
}

} // namespace Math

using AABB2D = Math::AABB<Vec2>;
using AABB2Di = Math::AABB<Vec2i>;
using AABB = Math::AABB<Vec3>;

#endif /* __AABB_HPP__ */