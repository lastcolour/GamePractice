#ifndef __ASYNC_ASSETS_LOADER_HPP__
#define __ASYNC_ASSETS_LOADER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Core/ETAssets.hpp"

struct AsyncLoadRequest {
    std::function<void(Memory::Buffer&)> callback;
    std::string filename;
    bool local;
};

class AsyncAssetsLoader : public SystemLogic,
    public ETNode<ETAssetsUpdateTask>,
    public ETNode<ETAsyncAssets> {
public:

    AsyncAssetsLoader();
    virtual ~AsyncAssetsLoader();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAsyncAssets
    void ET_asyncSaveLocalFile(const char* fileName, Memory::Buffer& buff) override;
    void ET_asyncLoadLocalFile(const char* fileName, std::function<void(Memory::Buffer&)> callback) override;
    void ET_asyncLoadAsset(const char* fileName, std::function<void(Memory::Buffer&)> callback) override;
    void ET_processAsyncLoadRequest(const AsyncLoadRequest& req) override;

    // ETAssetsUpdateTask
    void ET_updateAssets(float dt) override;
};

#endif /* __ASYNC_ASSETS_LOADER_HPP__ */