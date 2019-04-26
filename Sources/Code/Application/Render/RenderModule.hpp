#ifndef __RENDER_MODULE_HPP__
#define __RENDER_MODULE_HPP__

#include <Core/SystemModule.hpp>

class RenderModule : public SystemModule {
public:

    RenderModule();
    virtual ~RenderModule() = default;

protected:

    LogicsContainerPtrT getSystemLogics() const override;
};

#endif /* __RENDER_MODULE_HPP__ */