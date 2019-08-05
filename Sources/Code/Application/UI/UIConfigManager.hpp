#ifndef __UI_CONFIG_MANAGER_HPP__
#define __UI_CONFIG_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/UIETInterfaces.hpp"

class UIConfigManager : public SystemLogic,
    public ETNode<ETUIConfigManager> {
public:
    UIConfigManager();
    virtual ~UIConfigManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIConfigManager
    const char* ET_getDefaultGeomRenderer() override;
    const char* ET_getDefaultTextRenderer() override;
};

#endif /* __UI_CONFIG_MANAGER_HPP__ */