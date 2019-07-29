#include "Platforms/Desktop/DesktopAssets.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <chrono>
#include <fstream>
#include <cassert>
#include <algorithm>

#include <sys/stat.h>

#ifdef APP_BUILD_PLATFORM_WINDOWS
  #include <direct.h>
  #define GetCurrentWorkingDir _getcwd
  const char* CWD_PREFIX = "";
#elif defined APP_BUILD_PLATFORM_LINUX
  #include <unistd.h>
  #define GetCurrentWorkingDir getcwd
  const char* CWD_PREFIX = "/";
#else
  #error Neither APP_BUILD_PLATFORM_WINDOWS nor APP_BUILD_PLATFORM_LINUX is specified
#endif

namespace {

    const char VALID_SLASH = '/';

    bool isDirExist(const char* dirName) {
        if(!dirName || !dirName[0]) {
            return false;
        }
        struct stat info;
        if(stat(dirName, &info) != 0) {
            return false;
        } else if(info.st_mode & S_IFDIR) {
            return true;
        }
        return false;
    }

    void fixSlashes(std::string& origPath) {
        std::replace(origPath.begin(), origPath.end(), '\\', VALID_SLASH);
    }

    void normalizePath(std::string& origPath) {
        std::vector<std::string> tokens;
        size_t lastPos = 0u;
        for(size_t i=0u, sz=origPath.size(); i<sz; ++i) {
            if(origPath[i] == VALID_SLASH) {
                tokens.push_back(origPath.substr(lastPos, i-lastPos));
                lastPos = ++i;
            }
        }
        tokens.push_back(origPath.substr(lastPos, origPath.size()-lastPos));
        std::vector<std::string> resPath;
        resPath.reserve(tokens.size());
        for(auto& token : tokens) {
            if(token == ".") {
                continue;
            } else if(token == "..") {
                if(resPath.empty()) {
                    LogError("[DesktopAssets] Can't normalize path '%s'", origPath);
                    return;
                }
                resPath.pop_back();
            } else {
                resPath.emplace_back(std::move(token));
            }
        }
        origPath.clear();
        for(const auto& token : resPath) {
            if(!origPath.empty()) {
                origPath += VALID_SLASH;
            }
            origPath += token;
        }
    }

    std::string getAssetDirPath() {
        std::string cwdPath;
        {
            char temp[FILENAME_MAX];
            if(GetCurrentWorkingDir(temp, sizeof(temp))) {
                temp[sizeof(temp) - 1] = '\0';
                cwdPath = temp;
                cwdPath = CWD_PREFIX + cwdPath;
            } else {
                return "";
            }
        }

        std::vector<std::string> possiblePaths = {
            "/Assets",
            "/../Assets",
            "/../../Assets",
            "/../../../Assets",
        };

        for(const auto& path : possiblePaths) {
            auto dirPath = cwdPath + path;
            fixSlashes(dirPath);
            normalizePath(dirPath);
            if (isDirExist(dirPath.c_str())) {
                return dirPath;
            }
        }

        return "";
    }

    std::string transformToAssetPath(const std::string& assetRootPath, const std::string& assetPath) {
        if(assetPath.empty()) {
            return "";
        }
        std::string internalAssetName;
        if(assetPath[0] == '\\' || assetPath[0] == '/') {
            internalAssetName.assign(assetPath.substr(1, assetPath.size()-1));
        } else {
            internalAssetName = assetPath;
        }
        if(internalAssetName.empty()) {
            return "";
        }
        fixSlashes(internalAssetName);
        return StringFormat("%s%c%s", assetRootPath, VALID_SLASH, internalAssetName);
    }

} // namespace

bool DesktopAssets::init() {
    assetRootPath = getAssetDirPath();
    if(assetRootPath.empty()) {
        LogError("[DesktopAssets::onInit] Can't get assets root dir");
        return false;
    }
    ETNode<ETAssets>::connect(getEntityId());
    return true;
}

void DesktopAssets::deinit() {
    ETNode<ETAssets>::disconnect();
}

JSONNode DesktopAssets::ET_loadJSONAsset(const char* assetName) {
    auto buffer = ET_loadAsset(assetName);
    if(!buffer) {
        LogError("[DesktopAssets::loadJSONAsset] Can't load asset from: %s", assetName);
        return JSONNode();
    }
    auto rootNode = JSONNode::ParseBuffer(buffer);
    if(!rootNode) {
        LogError("[DesktopAssets::loadJSONAsset] Can't parse asset: %s", assetName);
        return JSONNode();
    }
    return rootNode;
}

Buffer DesktopAssets::ET_loadAsset(const char* assetName) {
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
        LogDebug("[DesktopAssets] Loaded file '%s' in %d ms", assetName,
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - loadStartT).count());
    }

    return buff;
}

Buffer DesktopAssets::loadAssetImpl(const std::string& assetName) {
    std::string assetPath = transformToAssetPath(assetRootPath, assetName);
    if(assetPath.empty()) {
        LogError("[DesktopAssets] Can't load file by invalid path: '%s'", assetName);
        return Buffer();
    }
    std::ifstream fin(assetPath, std::ios::binary | std::ios::ate);
    if(!fin.good() || !fin.is_open()) {
        LogError("[DesktopAssets] Can't load file: '%s'", assetPath);
        return Buffer();
    }
    long int fileSize = fin.tellg();
    if(fileSize == -1) {
        LogError("[DesktopAssets] Can't get file size: '%s'", assetName);
        return Buffer();
    }
    fin.seekg(0u, std::ios::beg);

    Buffer buffer(fileSize);
    if(!buffer) {
        LogError("[DesktopAssets] Can't allocate buffer of size %d to load file: '%s'", fileSize, assetPath);
        return Buffer();
    }
    if(!fin.read(static_cast<char*>(buffer.getWriteData()), fileSize)) {
        LogError("[DesktopAssets] Can't read file: '%s'", assetPath);
        return Buffer();
    }
    return buffer;
}