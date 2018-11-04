#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

class Platform {
public:

    virtual ~Platform() {}
    virtual bool init() = 0;
};

#endif /* __PLATFORM_HPP__ */