#include "Platforms/Desktop/DesktopAssets.hpp"
#include "Logger.hpp"

#include <chrono>
#include <fstream>
#include <cassert>
#include <algorithm>

#ifdef APP_BUILD_PLATFORM_WINDOWS
  #include <direct.h>
  #define GetCurrentWorkingDir _getcwd
#elif defined APP_BUILD_PLATFORM_LINUX
  #include <unistd.h>
  #define GetCurrentWorkingDir getcwd
#else
  #error Neither APP_BUILD_PLATFORM_WINDOWS nor APP_BUILD_PLATFORM_LINUX is specified
#endif

#ifndef APP_ASSETS_ROOT_DIR
  #error APP_ASSETS_ROOT_DIR not defined
#endif

namespace {

    const char VALID_SLASH = '/';

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
        char cwd[FILENAME_MAX];
        if(GetCurrentWorkingDir(cwd, sizeof(cwd))) {
            cwd[sizeof(cwd) - 1] = '\0';
        } else {
            assert(false && "Can't get app run dir");
            return ".";
        }
        std::string tPath(cwd);
        if(tPath.back() != '\\' || tPath.back() != '/') {
            tPath += VALID_SLASH;
        }
        tPath += APP_ASSETS_ROOT_DIR;
        if(!tPath.empty() && (tPath.back() != '\\' || tPath.back() != '/')) {
            tPath += VALID_SLASH;
        }
        fixSlashes(tPath);
        normalizePath(tPath);
        return tPath;
    }

    std::string transformToAssetPath(const std::string assetRootPath, const std::string& assetPath) {
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
    return !assetRootPath.empty();
}

Buffer DesktopAssets::loadAsset(const std::string& assetName) {
    auto loadStartT = std::chrono::high_resolution_clock::now();

    Buffer buff = loadAssetImpl(assetName);

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
    if(!fin.is_open()) {
        LogError("[DesktopAssets] Can't load file: '%s'", assetPath);
        return Buffer();
    }
    auto fileSize = fin.tellg();
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
    if(!fin.read(static_cast<char*>(buffer.getData()), fileSize)) {
        LogError("[DesktopAssets] Can't read file: '%s'", assetPath);
        return Buffer();
    }
    return buffer;
}