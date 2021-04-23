#ifndef __CYCLE_ARRAY_HPP__
#define __CYCLE_ARRAY_HPP__

#include <cassert>

template<typename T>
class CycleArray {
public:

    explicit CycleArray(size_t size) :
        data(size),
        pos(0),
        count(0) {
    }

    CycleArray(const CycleArray& other) :
        data(other.data),
        pos(other.pos),
        count(other.count) {}

    CycleArray(const CycleArray&& other) :
        data(std::move(other.data)),
        pos(other.pos),
        count(other.count) {

        other.pos = 0;
        other.count = 0;
    }

    CycleArray& operator=(const CycleArray& other) {
        if(this != &other) {
            data = other.data;
            pos = other.pos;
            count = other.count;
        }
        return *this;
    }

    CycleArray& operator=(CycleArray&& other) {
        if(this != &other) {
            data = std::move(other.data);
            pos = other.pos;
            other.pos = 0;
            count = other.count;
            other.count = 0;
        }
        return *this;
    }

    ~CycleArray() = default;

    size_t size() const {
        return count;
    }

    size_t maxSize() const {
        return data.size();
    }

    T& operator[](size_t idx) {
        assert(idx >= 0 && idx < count  && "Invalid elem access");
        auto i = (pos + idx) % count;
        return data[i];
    }

    const T& operator[](size_t idx) const {
        assert(idx >= 0 && idx < count  && "Invalid elem access");
        auto i = (pos + idx) % count;
        return data[i];
    }

    void insert(const T& elem) {
        data[pos] = elem;
        pos = pos + 1;
        count = std::min(count + 1, data.size());
        pos %= data.size();
    }

private:

    std::vector<T> data;
    size_t pos;
    size_t count;
};

#endif /* __CYCLE_ARRAY_HPP__ */