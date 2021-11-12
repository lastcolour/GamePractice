#include "Desktop/DesktopAssets.hpp"
#include "Core/JSONNode.hpp"
#include "Core/TimePoint.hpp"
#include "FileUtils.hpp"

#include <cassert>

#ifdef PLATFORM_WINDOWS
  #include <direct.h>
  #include <windows.h>
#elif defined PLATFORM_LINUX
  #include <unistd.h>
#else
  #error Neither PLATFORM_WINDOWS nor PLATFORM_LINUX is specified
#endif

namespace {

const char* LOCAL_DIR_PATH = "Local";

std::filesystem::path getBinDir() {
#ifdef PLATFORM_WINDOWS
    wchar_t cPathStr[FILENAME_MAX] = { 0 };
    GetModuleFileNameW(nullptr, &cPathStr[0], FILENAME_MAX);
    std::wstring binFilePath(cPathStr);
#else
    char cPathStr[FILENAME_MAX] = { 0 };
    std::string binFilePath;
    auto cPathStrLen = readlink("/proc/self/exe", cPathStr, FILENAME_MAX);
    binFilePath = std::string(cPathStr, (cPathStrLen > 0) ? cPathStrLen : 0);
#endif
    std::filesystem::path resPath(binFilePath, std::filesystem::path::format::generic_format);
    return resPath.parent_path();
}

std::filesystem::path getAssetDirPath() {
    std::filesystem::path searchPath;
    if(!ET_IsExistNode<ETAssetsSetup>()) {
        searchPath = getBinDir();
        if(searchPath.empty()) {
            LogError("[DesktopAssets::getAssetDirPath] Can't get current binary dir path");
            return std::filesystem::path();
        }
    } else {
        std::string strSearchPath;
        ET_SendEvent(&ETAssetsSetup::ET_getAssetsSearchPath, strSearchPath);
        searchPath = strSearchPath;
        if(searchPath.empty()) {
            LogError("[DesktopAssets::getAssetDirPath] Empty assets dir search path");
            return std::filesystem::path();
        }
    }
    std::vector<std::string> possiblePaths = {
        "Assets",
        "../Assets",
        "../../Assets",
        "../../../../Assets"
    };
    for(auto& possiblePath : possiblePaths) {
        auto possibleAssetDir = searchPath;
        possibleAssetDir.append(possiblePath);
        std::error_code errCode;
        if(std::filesystem::exists(possibleAssetDir, errCode)) {
            auto resPath = std::filesystem::weakly_canonical(possibleAssetDir);
            return resPath;
        }
    }
    LogError("[DesktopAssets::getAssetDirPath] Can't find assets root dir from: '%s'",
        searchPath.string());
    return std::filesystem::path();
}

std::filesystem::path getLoacalFilesDirPath() {
    std::filesystem::path path;
    if(!ET_IsExistNode<ETAssetsSetup>()) {
        path = getBinDir();
        if(path.empty()) {
            LogError("[DesktopAssets::getLoacalFilesDirPath] Can't get current binary dir path");
            return std::filesystem::path();
        }
    } else {
        std::string localDataRootPath;
        ET_SendEvent(&ETAssetsSetup::ET_getLocalDataRootPath, localDataRootPath);
        path = localDataRootPath;
        if(path.empty()) {
            LogError("[DesktopAssets::getLoacalFilesDirPath] Empty local data root path");
            return std::filesystem::path();
        }
    }
    path.append(LOCAL_DIR_PATH);
    if(!FileUtils::IsDirExists(path)) {
        if(!FileUtils::CreateDir(path)) {
            LogError("[DesktopAssets::getLoacalFilesDirPath] Can't create dir for local data: '%s'",
                path.string());
            return std::filesystem::path();
        }
    }
    return path;
}

} // namespace

bool DesktopAssets::init() {
    assetRootPath = getAssetDirPath();
    if(assetRootPath.empty()) {
        return false;
    }
    localRootPath = getLoacalFilesDirPath();
    if(localRootPath.empty()) {
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
        return JSONNode();
    }
    auto rootNode = JSONNode::ParseBuffer(buffer);
    if(!rootNode) {
        LogError("[DesktopAssets::loadJSONAsset] Can't parse asset: %s", assetName);
        return JSONNode();
    }
    return rootNode;
}

Memory::Buffer DesktopAssets::ET_loadAsset(const char* assetName) {
    auto normalAssetName = FileUtils::NormilizeAssetName(assetName);
    Memory::Buffer buff;
    if(normalAssetName.empty()) {
        return buff;
    }
    ET_SendEventReturn(buff, &ETAssetsCacheManager::ET_getAssetFromCache, normalAssetName.c_str());
    if(buff) {
        return buff;
    }

    auto loadStartT = TimePoint::GetNowTime();

    buff = FileUtils::LoadFileFromDir(assetRootPath, normalAssetName.c_str());

    if(buff) {
        ET_SendEvent(&ETAssetsCacheManager::ET_putAssetToCache, normalAssetName.c_str(), buff);
    }

    if(buff) {
        float msValue = -loadStartT.getMiliSecElapsedFrom(TimePoint::GetNowTime());
        LogDebug("[DesktopAssets::ET_loadAsset] Loaded file '%s' in %.1f ms", normalAssetName, msValue);
    }

    return buff;
}

Memory::Buffer DesktopAssets::ET_loadLocalFile(const char* fileName) {
    return FileUtils::LoadFileFromDir(localRootPath, fileName);
}

JSONNode DesktopAssets::ET_loadLocalJSONFile(const char* fileName) {
    auto buffer = ET_loadLocalFile(fileName);
    if(!buffer) {
        return JSONNode();
    }
    auto rootNode = JSONNode::ParseBuffer(buffer);
    if(!rootNode) {
        LogError("[DesktopAssets::ET_loadLocalJSONFile] Can't parse local file: %s", fileName);
        return JSONNode();
    }
    return rootNode;
}

bool DesktopAssets::ET_saveLocalFile(const char* fileName, const Memory::Buffer& buff) {
    return FileUtils::SaveFileToDir(localRootPath, fileName, buff);
}

bool DesktopAssets::ET_removeLocalFile(const char* fileName) {
    return FileUtils::RemoveFileFromDir(localRootPath, fileName);
}

bool DesktopAssets::ET_isLocalFileExists(const char* fileName) const {
    auto path = FileUtils::CombinePath(localRootPath, fileName);
    return FileUtils::IsFileExists(path);
}