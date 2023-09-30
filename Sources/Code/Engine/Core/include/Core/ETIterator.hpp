#ifndef __ET_ITERATOR_HPP__
#define __ET_ITERATOR_HPP__

#include "Core/ETUtils.hpp"

namespace ET {

class ETNodeBase {
protected:
    virtual ~ETNodeBase() = default;
};

class ETIterator {
public:

    using ContainerT = std::unordered_multimap<EntityId, ETNodeBase*>;
    using IteratorT = typename ContainerT::iterator;

public:

    ETIterator(const IteratorT& startIt, const IteratorT& endIt, int etIndex) :
        current(startIt),
        end(endIt),
        etId(etIndex) {

        while(current != end && !current->second) {
            ++current;
        }
    }

    ETIterator(ETIterator&& other) :
        current(std::move(other.current)),
        end(std::move(other.end)),
        etId(other.etId) {
    }

    ETIterator& operator=(ETIterator&& other) {
        if(this != &other) {
            current = std::move(other.current);
            end = std::move(other.end);
            etId = etId;
        }
        return *this;
    }

    ~ETIterator() {
    }

    operator bool() const {
        return current != end;
    }

    ETNodeBase* operator*() {
        return current->second;
    }

    void operator++() {
        do {
            ++current;
        } while(current != end && !current->second);
    }

    int getETId() const { return etId; }

private:

    IteratorT current;
    IteratorT end;
    int etId;
};

} // namespace ET

#endif /* __ET_ITERATOR_HPP__ */