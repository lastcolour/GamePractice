#include "Core/App/AsyncAssetsLoader.hpp"

#include <cassert>

AsyncAssetsLoader::AsyncAssetsLoader() {
}

AsyncAssetsLoader::~AsyncAssetsLoader() {
}

bool AsyncAssetsLoader::init() {
    ETNode<ETAsyncAssets>::connect(getEntityId());
    ETNode<ETAssetsUpdateTask>::connect(getEntityId());
    return true;
}

void AsyncAssetsLoader::deinit() {
}

void AsyncAssetsLoader::ET_updateAssets(float dt) {
    ET_PollAllEvents<ETAssets>();
    ET_PollAllEvents<ETAsyncAssets>();
}

void AsyncAssetsLoader::ET_asyncSaveLocalFile(const char* fileName, Buffer& buff) {
    ET_QueueEvent(&ETAssets::ET_saveLocalFile, fileName, buff);
}

void AsyncAssetsLoader::ET_asyncLoadLocalFile(const char* fileName, std::function<void(Buffer&)> callback) {
    if(!fileName || !fileName[0]) {
        LogWarning("[AsyncAssetsLoader::ET_asyncLoadLocalFile] Can't load file with empty name");
        return;
    }
    if(!callback) {
        assert(false && "Can't do async load without callback");
        return;
    }
    AsyncLoadRequest req;
    req.filename = fileName;
    req.local = true;
    req.callback = callback;
    ET_QueueEvent(&ETAsyncAssets::ET_processAsyncLoadRequest, req);
}

void AsyncAssetsLoader::ET_asyncLoadAsset(const char* fileName, std::function<void(Buffer&)> callback) {
    if(!fileName || !fileName[0]) {
        LogWarning("[AsyncAssetsLoader::ET_asyncLoadAsset] Can't load file with empty name");
        return;
    }
    if(!callback) {
        assert(false && "Can't do async load without callback");
        return;
    }
    AsyncLoadRequest req;
    req.filename = fileName;
    req.local = false;
    req.callback = callback;
    ET_QueueEvent(&ETAsyncAssets::ET_processAsyncLoadRequest, req);
}

void AsyncAssetsLoader::ET_processAsyncLoadRequest(const AsyncLoadRequest& req) {
    Buffer buff;
    if(req.local) {
        ET_SendEventReturn(buff, &ETAssets::ET_loadLocalFile, req.filename.c_str());
    } else {
        ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, req.filename.c_str());
    }
    if(!buff) {
        LogWarning("[AsyncAssetsLoader::ET_processAsyncLoadRequest] Can't async load file: '%s'", req.filename);
    }
    req.callback(buff);
}