#ifndef __AABB_HPP__
#define __AABB_HPP__

namespace Math {

template<typename T>
struct AABB2D {

    using ElemTypeT = typename T::ValueType;

public:

    AABB2D() = default;
    AABB2D(const T& bt, const T& tp) :
        bot(bt), top(tp) {}
    AABB2D(const AABB2D& other) :
        bot(other.bot), top(other.top) {}
    explicit AABB2D(const ElemTypeT& val) :
        bot(val), top(val) {}

    AABB2D& operator=(const AABB2D& other) {
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

    T getArea() const {
        return (top.x - bot.x) * (top.y - bot.y);
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

    void setCenterAndSize(const T& c, const T& sz) {
        bot = c - sz / (ElemTypeT)2;
        top = bot + sz;
    }

    AABB2D expandBy(const T& v) const {
        return AABB2D{bot - v, top + v};
    }

    AABB2D expandBy(const ElemTypeT& v) const {
        return expandBy(T(v));
    }

public:

    T bot;
    T top;
};

template<typename T>
AABB2D<T> operator+(const AABB2D<T>& aabb, const T& pos) {
    return AABB2D<T>(aabb.bot + pos, aabb.top + pos);
}

template<typename T>
bool operator==(const AABB2D<T>& first, const AABB2D<T>& second) {
    return first.top == second.top && first.bot == second.bot;
}

template<typename T>
bool operator!=(const AABB2D<T>& first, const AABB2D<T>& second) {
    return !(first == second);
}

} // namespace Math

using AABB2D = Math::AABB2D<Vec2>;
using AABB2Di = Math::AABB2D<Vec2i>;

#endif /* __AABB_HPP__ */