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
  #include <windows.h>
  #define PLATFORM_GET_CWD _getcwd
  #define PLATFORM_CREATE_DIR(DIR_NAME) CreateDirectory(DIR_NAME, nullptr)
  const char* CWD_PREFIX = "";
#elif defined APP_BUILD_PLATFORM_LINUX
  #include <unistd.h>
  #define PLATFORM_GET_CWD getcwd
  #define PLATFORM_CREATE_DIR(DIR_NAME) !mkdir(DIR_NAME)
  const char* CWD_PREFIX = "/";
#else
  #error Neither APP_BUILD_PLATFORM_WINDOWS nor APP_BUILD_PLATFORM_LINUX is specified
#endif

namespace {

const char VALID_SLASH = '/';
const char* LOCAL_DIR_PATH = "Local";

std::string getCWD() {
    std::string cwdPath;
    char temp[FILENAME_MAX];
    if(PLATFORM_GET_CWD(temp, sizeof(temp))) {
        temp[sizeof(temp) - 1] = '\0';
        cwdPath = temp;
        cwdPath = CWD_PREFIX + cwdPath;
    }
    return cwdPath;
}

bool createDir(const std::string& dirName) {
    if(!PLATFORM_CREATE_DIR(dirName.c_str())) {
        return false;
    }
    return true;
}

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
    std::string cwdPath = getCWD();
    if(cwdPath.empty()) {
        return "";
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
        if (isDirExist(dirPath)) {
            return dirPath;
        }
    }
    return "";
}

std::string getLoacalFilesDirPath() {
    std::string cwdPath = getCWD();
    if(cwdPath.empty()) {
        return false;
    }
    fixSlashes(cwdPath);
    std::string dirPath = StringFormat("%s%c%s", cwdPath, VALID_SLASH, LOCAL_DIR_PATH);
    if(!isDirExist(dirPath)) {
        if(!createDir(dirPath)) {
            return false;
        }
    }
    return dirPath;
}

std::string transformToPath(const std::string& dirPath, const std::string& filePath) {
    if(filePath.empty()) {
        return "";
    }
    std::string internalFileName;
    if(filePath[0] == '\\' || filePath[0] == '/') {
        internalFileName.assign(filePath.substr(1, filePath.size()-1));
    } else {
        internalFileName = filePath;
    }
    if(internalFileName.empty()) {
        return "";
    }
    fixSlashes(internalFileName);
    return StringFormat("%s%c%s", dirPath, VALID_SLASH, internalFileName);
}

} // namespace

bool DesktopAssets::init() {
    assetRootPath = getAssetDirPath();
    if(assetRootPath.empty()) {
        LogError("[DesktopAssets::onInit] Can't get assets root dir");
        return false;
    }
    localRootPath = getLoacalFilesDirPath();
    if(localRootPath.empty()) {
        LogError("[DesktopAssets::onInit] Can't get local root dir");
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

    buff = loadFileFromDir(assetRootPath, assetName);

    if(buff) {
        ET_SendEvent(&ETAssetsCacheManager::ET_putAssetToCache, assetName, buff);
    }

    if(buff) {
        LogDebug("[DesktopAssets::ET_loadAsset] Loaded file '%s' in %d ms", assetName,
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - loadStartT).count());
    }

    return buff;
}

Buffer DesktopAssets::ET_loadLocalFile(const char* fileName) {
    Buffer buff;
    if(!fileName || !fileName[0]) {
        return buff;
    }
    buff = loadFileFromDir(localRootPath, fileName);
    return buff;
}

JSONNode DesktopAssets::ET_loadLocalJSONFile(const char* fileName) {
    auto buffer = ET_loadLocalFile(fileName);
    if(!buffer) {
        LogError("[DesktopAssets::ET_loadLocalJSONFile] Can't load local file from: %s", fileName);
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
    if(isDirExist(filePath.c_str())) {
        LogError("[DesktopAssets::ET_saveLocalFile] Can't write data to a dir object: '%s'", filePath);
        return false;
    }
    std::ofstream fout(filePath, std::ios::out | std::ios::binary);
    if(!fout.good() || !fout.is_open()) {
        LogError("[DesktopAssets::ET_saveLocalFile] Can't create/open a file: '%s'", filePath);
        return false;
    }
    if(!fout.write(static_cast<const char*>(buff.getReadData()), buff.getSize())) {
        LogError("[DesktopAssets::ET_saveLocalFile] Can't write to the file: '%s'", filePath);
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
    if(isDirExist(filePath.c_str())) {
        LogError("[DesktopAssets::ET_removeLocalFile] Can't remove a dir object: '%s'", filePath);
        return false;
    }
    if(remove(filePath.c_str())) {
        LogError("[DesktopAssets::ET_removeLocalFile] Unable to remove file '%s'", filePath);
        return false;
    }
    return true;
}

Buffer DesktopAssets::loadFileFromDir(const std::string& dirPath, const std::string& fileName) {
    std::string filePath = transformToPath(dirPath, fileName);
    if(filePath.empty()) {
        LogError("[DesktopAssets::loadFileFromDir] Can't load file by invalid name: '%s'", fileName);
        return Buffer();
    }
    if(isDirExist(filePath.c_str())) {
        LogError("[DesktopAssets::loadFileFromDirImpl] Can't opend dir as file: '%s'", filePath);
        return Buffer();
    }
    std::ifstream fin(filePath, std::ios::binary | std::ios::ate);
    if(!fin.good() || !fin.is_open()) {
        LogError("[DesktopAssets::loadFileFromDir] Can't load file: '%s'", filePath);
        return Buffer();
    }
    std::streamoff fileSize = fin.tellg();
    if(fileSize == -1) {
        LogError("[DesktopAssets::loadFileFromDir] Can't get file size: '%s'", filePath);
        return Buffer();
    }
    fin.seekg(0u, std::ios::beg);
    Buffer buffer(fileSize);
    if(!buffer) {
        LogError("[DesktopAssets::loadFileFromDir] Can't allocate buffer of size %d to load file: '%s'", fileSize, filePath);
        return Buffer();
    }
    if(!fin.read(static_cast<char*>(buffer.getWriteData()), fileSize)) {
        LogError("[DesktopAssets::loadFileFromDir] Can't read file: '%s'", filePath);
        return Buffer();
    }
    return buffer;
}

bool DesktopAssets::saveFileToDir(const std::string& dirPath, const std::string& fileName) {
    return false;
}