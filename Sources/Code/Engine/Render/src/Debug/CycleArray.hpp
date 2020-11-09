#ifndef __CYCLE_ARRAY_HPP__
#define __CYCLE_ARRAY_HPP__

#include <vector>

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

    size_t getPos() const {
        return pos;
    }

    T& operator[](size_t idx) {
        return data[idx];
    }

    const T& operator[](size_t idx) const {
        return data[idx];
    }

    void insert(T& elem) {
        pos += 1;
        count = std::max(count, pos);
        pos %= data.size();
        data[pos] = elem;
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