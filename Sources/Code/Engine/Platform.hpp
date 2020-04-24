#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

#include <memory>

#include "Core/SystemModule.hpp"

class Platform {
public:
    virtual ~Platform() = default;
    virtual std::unique_ptr<SystemModule> createPlatformModule() const = 0;
};

#endif /* __PLATFORM_HPP__ */