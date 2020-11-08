#include "Desktop/DesktopAssets.hpp"
#include "Core/ETLogger.hpp"
#include "Core/JSONNode.hpp"
#include "Core/TimePoint.hpp"

#include <fstream>
#include <cassert>
#include <algorithm>
#include <filesystem>

#ifdef PLATFORM_WINDOWS
  #include <direct.h>
  #include <windows.h>
  const char* CWD_PREFIX = "";
#elif defined PLATFORM_LINUX
  #include <unistd.h>
  const char* CWD_PREFIX = "/";
#else
  #error Neither PLATFORM_WINDOWS nor PLATFORM_LINUX is specified
#endif

namespace {

const char* LOCAL_DIR_PATH = "Local";
const size_t MAX_ERROR_MSG_LEN = 128;

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

bool isDirExist(const std::filesystem::path& path) {
    std::error_code errCode;
    if(!std::filesystem::exists(path, errCode)) {
        if(errCode) {
            LogError("Can't query dir status: %s (Error: %s)", path.string(), errCode.message());
        }
        return false;
    }
    errCode.clear();
    if(!std::filesystem::is_directory(path, errCode)) {
        if(errCode) {
            LogError("Can't query file info: %s (Error: %s)", path.string(), errCode.message());
        }
        return false;
    }
    return true;
}

bool createDir(const std::filesystem::path& path) {
    std::error_code errCode;
    if(!std::filesystem::create_directory(path, errCode)) {
        if(errCode) {
            LogError("Can't create directory: %s (Error: %s)", path.string(), errCode.message());
        } else {
            LogError("Can't create directory: %s (Error: Unknown)", path.string());
        }
        return false;
    }
    return true;
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

std::string GetSafeStrErrno() {
#ifdef PLATFORM_WINDOWS
    char cErrorMsg[MAX_ERROR_MSG_LEN];
    strerror_s(&(cErrorMsg[0]), MAX_ERROR_MSG_LEN, errno);
    return &(cErrorMsg[0]);
#else
    return strerror(errno);
#endif
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
    if(!isDirExist(path)) {
        if(!createDir(path)) {
            LogError("[DesktopAssets::getLoacalFilesDirPath] Can't create dir for local data: '%s'",
                path.string());
            return std::filesystem::path();
        }
    }
    return path;
}

std::filesystem::path transformToPath(const std::filesystem::path& dirPath, const std::string& filePath) {
    std::filesystem::path resPath = dirPath;
    resPath.append(filePath);
    resPath = resPath.make_preferred();
    return resPath;
}

std::string normilizeAssetName(const char* assetPath) {
    if(!assetPath || !assetPath[0]) {
        return std::string();
    }
    std::string path = assetPath;
    for(auto& ch : path) {
        if(ch == '\\') {
            ch = '/';
        }
    }
    if(path[0] == '/') {
        path.assign(path.substr(1, path.size()-1));
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

Buffer DesktopAssets::ET_loadAsset(const char* assetName) {
    auto normalAssetName = normilizeAssetName(assetName);
    Buffer buff;
    if(normalAssetName.empty()) {
        return buff;
    }
    ET_SendEventReturn(buff, &ETAssetsCacheManager::ET_getAssetFromCache, normalAssetName.c_str());
    if(buff) {
        return buff;
    }

    auto loadStartT = TimePoint::GetNowTime();

    buff = loadFileFromDir(assetRootPath, normalAssetName);

    if(buff) {
        ET_SendEvent(&ETAssetsCacheManager::ET_putAssetToCache, normalAssetName.c_str(), buff);
    }

    if(buff) {
        int msValue = -loadStartT.getMiliSecElapsedFrom(TimePoint::GetNowTime());
        LogDebug("[DesktopAssets::ET_loadAsset] Loaded file '%s' in %d ms", normalAssetName, msValue);
    }

    return buff;
}

Buffer DesktopAssets::ET_loadLocalFile(const char* fileName) {
    if(!fileName || !fileName[0]) {
        LogError("[DesktopAssets::ET_loadLocalFile] Can't local file with empty name");
        return Buffer();
    }
    Buffer buff = loadFileFromDir(localRootPath, fileName);
    return buff;
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

bool DesktopAssets::ET_saveLocalFile(const char* fileName, const Buffer& buff) {
    auto filePath = transformToPath(localRootPath, fileName);
    if(filePath.empty()) {
        LogError("[DesktopAssets::ET_saveLocalFile] Can't save a file by invalid name: '%s'", fileName);
        return false;
    }
    if(isDirExist(filePath)) {
        LogError("[DesktopAssets::ET_saveLocalFile] Can't write data to a dir object: '%s'", filePath.string());
        return false;
    }
    std::ofstream fout(filePath, std::ios::out | std::ios::binary);
    if(!fout.good() || !fout.is_open()) {
        LogError("[DesktopAssets::ET_saveLocalFile] Can't create/open a file: '%s' (Error: %s)", filePath.string(), GetSafeStrErrno());
        return false;
    }
    if(!fout.write(static_cast<const char*>(buff.getReadData()), buff.getSize())) {
        LogError("[DesktopAssets::ET_saveLocalFile] Can't write to the file: '%s' (Error: %s)", filePath.string(), GetSafeStrErrno());
        return false;
    }
    return true;
}

bool DesktopAssets::ET_removeLocalFile(const char* fileName) {
    auto filePath = transformToPath(localRootPath, fileName);
    if(filePath.empty()) {
        LogError("[DesktopAssets::ET_removeLocalFile] Can't remove a file by invalid name: '%s'", fileName);
        return false;
    }
    if(isDirExist(filePath)) {
        LogError("[DesktopAssets::ET_removeLocalFile] Can't remove a dir object: '%s'", filePath.string());
        return false;
    }
    std::error_code errCode;
    if(!std::filesystem::remove(filePath, errCode)) {
        if(errCode) {
            LogError("[DesktopAssets::ET_removeLocalFile] Unable to remove file '%s' (Error: %s)", filePath.string(), errCode.message());
        } else {
            LogError("[DesktopAssets::ET_removeLocalFile] Unable to remove file '%s' (Error: Unknown)", filePath.string());
        }
        return false;
    }
    return true;
}

Buffer DesktopAssets::loadFileFromDir(const std::filesystem::path& dirPath, const std::string& fileName) {
    auto filePath = transformToPath(dirPath, fileName);
    if(filePath.empty()) {
        LogError("[DesktopAssets::loadFileFromDir] Can't load file by invalid name: '%s'", fileName);
        return Buffer();
    }
    if(isDirExist(filePath)) {
        LogError("[DesktopAssets::loadFileFromDir] Can't opend dir as file: '%s'", filePath.string());
        return Buffer();
    }
    std::ifstream fin(filePath, std::ios::binary | std::ios::ate);
    if(!fin.good() || !fin.is_open()) {
        LogError("[DesktopAssets::loadFileFromDir] Can't load file: '%s' (Error: %s)", filePath.string(), GetSafeStrErrno());
        return Buffer();
    }
    std::streamoff fileSize = fin.tellg();
    if(fileSize == -1) {
        LogError("[DesktopAssets::loadFileFromDir] Can't get file size: '%s'", filePath.string());
        return Buffer();
    }
    fin.seekg(0u, std::ios::beg);
    Buffer buffer(fileSize);
    if(!buffer) {
        LogError("[DesktopAssets::loadFileFromDir] Can't allocate buffer of size %d to load file: '%s'", fileSize, filePath.string());
        return Buffer();
    }
    if(!fin.read(static_cast<char*>(buffer.getWriteData()), fileSize)) {
        LogError("[DesktopAssets::loadFileFromDir] Can't read file: '%s'", filePath.string());
        return Buffer();
    }
    return buffer;
}