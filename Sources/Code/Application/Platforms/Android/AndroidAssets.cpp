#include "Platforms/Android/AndroidAssets.hpp"
#include "Platforms/Android/AndroidPlatformHandler.hpp"
#include "Core/JSONNode.hpp"

#include <fstream>

#include <android/asset_manager.h>
#include <sys/stat.h>

namespace {

const off_t READ_CHUNK_SIZE = 1024 * 1024;

bool isDirExist(const std::string& dirName) {
    if(dirName.empty()) {
        return false;
    }
    struct stat info;
    if(stat(dirName.c_str(), &info) != 0) {
        return false;
    } else if(info.st_mode & S_IFDIR) {
        return true;
    }
    return false;
}

bool createDir(const std::string& dirName) {
    if(dirName.empty()) {
        return false;
    }
    if(!mkdir(dirName.c_str(), S_IRWXU|S_IRWXG)) {
        return false;
    }
    return true;
}

} // namespace

AndroidAssets::AndroidAssets() {
}

AndroidAssets::~AndroidAssets() {
}

bool AndroidAssets::init() {
    ETNode<ETAssets>::connect(getEntityId());
    return true;
}

void AndroidAssets::deinit() {
    ETNode<ETAssets>::disconnect();
}

JSONNode AndroidAssets::ET_loadJSONAsset(const char* assetName) {
    auto buffer = ET_loadAsset(assetName);
    if(!buffer) {
        return JSONNode();
    }
    auto rootNode = JSONNode::ParseBuffer(buffer);
    if(!rootNode) {
        LogError("[AndroidAssets::loadJSONAsset] Can't parse asset: %s", assetName);
        return JSONNode();
    }
    return rootNode;
}

Buffer AndroidAssets::ET_loadAsset(const char* assetName) {
    Buffer buff;
    if(!assetName || !assetName[0]) {
        return buff;
    }
    ET_SendEventReturn(buff, &ETAssetsCacheManager::ET_getAssetFromCache, assetName);
    if (buff) {
        return buff;
    }

    auto loadStartT = std::chrono::high_resolution_clock::now();

    buff = loadAssetImpl(assetName);

    if(buff) {
        ET_SendEvent(&ETAssetsCacheManager::ET_putAssetToCache, assetName, buff);
    }

    if(buff) {
        LogDebug("[AndroidAssets::ET_loadAsset] Loaded file '%s' in %d ms", assetName,
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - loadStartT).count());
    }

    return buff;
}

Buffer AndroidAssets::loadAssetImpl(const std::string& assetName) {
    // Load from local file
    std::ifstream fin(assetName, std::ios::binary | std::ios::ate);
    if(fin.good() && fin.is_open()) {
        long int fileSize = fin.tellg();
        if(fileSize != -1) {
            fin.seekg(0u, std::ios::beg);
            Buffer buff(fileSize);
            if (buff && fin.read(static_cast<char*>(buff.getWriteData()), fileSize)) {
                LogDebug("[AndroidAssets::loadAssetImpl] Load asset from local file: %s", assetName);
                return buff;
            }
        }
    }

    // Load from apk
    AAssetManager* assetManager = GetAndroindPlatformHandler()->getAssetsManager();
    AAsset* androidAsset = AAssetManager_open(assetManager, assetName.c_str(), AASSET_MODE_BUFFER);
    if(!androidAsset) {
        LogError("[AndroidAssets::loadAssetImpl] Can't open asset: %s", assetName);
        return Buffer();
    }
    off_t assetSize = AAsset_getLength(androidAsset);
    Buffer buff(assetSize);
    if(!buff) {
        LogError("[AndroidAssets::loadAssetImpl] Can't allocate buffer of size %d to load asset %s",
            assetSize, assetName);
        AAsset_close(androidAsset);
        return Buffer();
    }
    off_t currOffset = 0;
    off_t readSize = 0;
    while((readSize = AAsset_read(androidAsset, static_cast<char*>(buff.getWriteData()) + currOffset, ::READ_CHUNK_SIZE)) > 0) {
        currOffset += readSize;
    }
    if(readSize < 0) {
        LogError("[AndroidAssets::loadAssetImpl] Unknow platform error when load asset %s", assetName);
        AAsset_close(androidAsset);
        return Buffer();
    }
    AAsset_close(androidAsset);
    return buff;
}

Buffer AndroidAssets::ET_loadLocalFile(const char* fileName) {
    std::string filePath = fileName;
    if(filePath.empty()) {
        LogError("[AndroidAssets::ET_loadLocalFile] Can't load a file with empty name");
        return Buffer();
    }
    if(isDirExist(filePath)) {
        LogError("[AndroidAssets::ET_loadLocalFile] Can't opend dir as file: %s", filePath);
        return Buffer();
    }
    std::ifstream fin(filePath, std::ios::binary | std::ios::ate);
    if(!fin.good() || !fin.is_open()) {
        LogError("[AndroidAssets::ET_loadLocalFile] Can't load file: '%s'. Error: %s", filePath, strerror(errno));
        return Buffer();
    }
    std::streamoff fileSize = fin.tellg();
    if(fileSize == -1) {
        LogError("[AndroidAssets::ET_loadLocalFile] Can't get file size: '%s'", filePath);
        return Buffer();
    }
    fin.seekg(0u, std::ios::beg);
    Buffer buffer(fileSize);
    if(!buffer) {
        LogError("[AndroidAssets::ET_loadLocalFile] Can't allocate buffer of size %d to load file: '%s'", fileSize, filePath);
        return Buffer();
    }
    if(!fin.read(static_cast<char*>(buffer.getWriteData()), fileSize)) {
        LogError("[AndroidAssets::ET_loadLocalFile] Can't read file: '%s'; Error: %s", filePath, strerror(errno));
        return Buffer();
    }
    return buffer;
}

JSONNode AndroidAssets::ET_loadLocalJSONFile(const char* fileName) {
    auto buffer = ET_loadLocalFile(fileName);
    if(!buffer) {
        return JSONNode();
    }
    auto rootNode = JSONNode::ParseBuffer(buffer);
    if(!rootNode) {
        LogError("[AndroidAssets::ET_loadLocalJSONFile] Can't parse file: %s", fileName);
        return JSONNode();
    }
    return rootNode;
}

bool AndroidAssets::ET_saveLocalFile(const char* fileName, const Buffer& buff) {
    if(!fileName || !fileName[0]) {
        LogError("[AndroidAssets::ET_saveLocalFile] Can't save file with empty name");
        return false;
    }
    std::string dataDirPath = GetAndroindPlatformHandler()->getInternalPath();
    if(!isDirExist(dataDirPath)) {
        LogDebug("[AndroidAssets::ET_saveLocalFile] Internal data dir doesn't exist: %s", dataDirPath);
        if(!createDir(dataDirPath)) {
            LogError("[AndroidAssets::ET_saveLocalFile] Can't create internal data %s dir to save file: %s; Error",
                dataDirPath, fileName, strerror(errno));
            return false;
        }
    }
    std::string filePath = dataDirPath + "/" + fileName;
    std::ofstream fout(filePath, std::ios::out | std::ios::binary);
    if(!fout.good() || !fout.is_open()) {
        LogError("[AndroidAssets::ET_saveLocalFile] Can't create/open a file: '%s'; Error: %s", filePath, strerror(errno));
        return false;
    }
    if(!fout.write(static_cast<const char*>(buff.getReadData()), buff.getSize())) {
        LogError("[AndroidAssets::ET_saveLocalFile] Can't write to the file: '%s'; Error: %s", filePath, strerror(errno));
        return false;
    }
    return true;
}

bool AndroidAssets::ET_removeLocalFile(const char* fileName) {
    auto filePath = StringFormat("%s/%s", GetAndroindPlatformHandler()->getInternalPath(), fileName);
    if(filePath.empty()) {
        LogError("[AndroidAssets::ET_removeLocalFile] Can't remove a file by invalid name: '%s'", fileName);
        return false;
    }
    if(isDirExist(filePath)) {
        LogError("[AndroidAssets::ET_removeLocalFile] Can't remove a dir object: '%s'", filePath);
        return false;
    }
    if(remove(filePath.c_str())) {
        LogError("[AndroidAssets::ET_removeLocalFile] Unable to remove file '%s'; Error: %s", filePath, strerror(errno));
        return false;
    }
    return true;
}