#ifndef __CYCLE_ARRAY_HPP__
#define __CYCLE_ARRAY_HPP__

#include <vector>

template<typename T>
class CycleArray {
public:

    CycleArray(size_t size) :
        data(size),
        pos(0),
        count(0) {
    }

    ~CycleArray() = default;

    size_t size() const {
        return count;
    }

    T& operator[](size_t idx) {
        return data[idx];
    }


    const T& operator[](size_t idx) const {
        return data[idx];
    }

    void insert(T& elem) {
        pos += 1;
        if(pos > count) {
            count = pos;
        }
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