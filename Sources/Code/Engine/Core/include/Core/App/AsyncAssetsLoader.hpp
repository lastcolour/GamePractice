#ifndef __ASYNC_ASSETS_LOADER_HPP__
#define __ASYNC_ASSETS_LOADER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Core/ETPrimitives.hpp"

class AsyncAssetsLoader : public SystemLogic,
    public ETNode<ETAssetsUpdateTask> {
public:

    AsyncAssetsLoader();
    virtual ~AsyncAssetsLoader();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAssetsUpdateTask
    void ET_updateAssets() override;

private:

};

#endif /* __ASYNC_ASSETS_LOADER_HPP__ */