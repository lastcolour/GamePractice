#ifndef __ANDROIND_MODULE_HPP__
#define __ANDROIND_MODULE_HPP__

#include "Core/SystemModule.hpp"

class AndroidModule : public SystemModule {
public:

    AndroidModule();
    virtual ~AndroidModule();

protected:

    ConfigsPtrT getSystemConfigs() const override;
    LogicsContainerPtrT getSystemLogics() const override;
};

#endif /* __ANDROIND_MODULE_HPP__ */