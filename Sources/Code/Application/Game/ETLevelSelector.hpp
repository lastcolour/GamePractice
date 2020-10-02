#ifndef __ET_LEVEL_SELECTOR_HPP__
#define __ET_LEVEL_SELECTOR_HPP__

struct ETLevelSelector {
    virtual ~ETLevelSelector() = default;
    virtual const char* ET_getLevelName() const = 0;
};

#endif /* __ET_LEVEL_SELECTOR_HPP__ */