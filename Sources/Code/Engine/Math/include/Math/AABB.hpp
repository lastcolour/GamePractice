#ifndef __AABB_HPP__
#define __AABB_HPP__

namespace Math {

template<typename T>
struct AABB {

    using ElemTypeT = typename T::ValueType;

public:

    AABB() = default;
    AABB(const T& bt, const T& tp) :
        bot(bt), top(tp) {}
    AABB(const AABB& other) :
        bot(other.bot), top(other.top) {}
    explicit AABB(const ElemTypeT& val) :
        bot(val), top(val) {}

    AABB& operator=(const AABB& other) {
        bot = other.bot;
        top = other.top;
        return *this;
    }

    T clamp(const ElemTypeT& x, const ElemTypeT& y) const {
        ElemTypeT tX = Math::Clamp(x, bot.x, top.x);
        ElemTypeT tY = Math::Clamp(y, bot.y, top.y);
        return T(tX, tY);
    }

    T clamp(const T& pt) const {
        return clamp(pt.x, pt.y);
    }

    bool isInside(const ElemTypeT& x, const ElemTypeT& y) const {
        return x >= bot.x && x <= top.x && y >= bot.y && y <= top.y;
    }

    bool isInside(const T& pos) const {
        return isInside(pos.x, pos.y);
    }

    T getSize() const {
        return top - bot;
    }

    T getCenter() const {
        return (top + bot) / static_cast<ElemTypeT>(2);
    }

    void setCenter(const ElemTypeT& x, const ElemTypeT& y) {
        T shift = T(x, y) - getCenter();
        bot += shift;
        top += shift;
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

#endif /* __AABB_HPP__ */