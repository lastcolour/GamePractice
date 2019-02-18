#ifndef __ASSETS_HPP__
#define __ASSETS_HPP__

#include "Buffer.hpp"

class Assets {
public:

    virtual ~Assets() = default;

    virtual bool init() = 0;
    virtual Buffer loadAsset(const std::string& assetName) = 0;
};

#endif /* __ASSETS_HPP__ */