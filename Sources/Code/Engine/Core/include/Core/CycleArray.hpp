#ifndef __CYCLE_ARRAY_HPP__
#define __CYCLE_ARRAY_HPP__

#include <vector>
#include <cassert>

template<typename T>
class CycleArray {
public:

    explicit CycleArray(size_t size) :
        data(size),
        pos(0),
        count(0) {
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

    CycleArray(const CycleArray&) = delete;
    CycleArray& operator=(const CycleArray&) = delete;

private:

    std::vector<T> data;
    size_t pos;
    size_t count;
};

#endif /* __CYCLE_ARRAY_HPP__ */