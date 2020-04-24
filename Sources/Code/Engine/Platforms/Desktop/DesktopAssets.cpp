#include "Platforms/Desktop/DesktopAssets.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <chrono>
#include <fstream>
#include <cassert>
#include <algorithm>

#include <sys/stat.h>

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

const char VALID_SLASH = '/';
const char* LOCAL_DIR_PATH = "Local";
const size_t MAX_ERROR_MSG_LEN = 128;

void fixSlashes(std::string& origPath) {
    std::replace(origPath.begin(), origPath.end(), '\\', VALID_SLASH);
}

std::string getBinDir() {
    char cPathStr[FILENAME_MAX] = { 0 };
    std::string binFilePath;
#ifdef PLATFORM_WINDOWS
    GetModuleFileName(nullptr, cPathStr, FILENAME_MAX);
    binFilePath = cPathStr;
#else 
    auto cPathStrLen = readlink("/proc/self/exe", cPathStr, FILENAME_MAX);
    binFilePath = std::string(cPathStr, (cPathStrLen > 0) ? cPathStrLen : 0);
#endif

    if(binFilePath.empty()) {
        return "";
    }

    fixSlashes(binFilePath);

    auto lastSlashPt = binFilePath.find_last_of(VALID_SLASH);
    if(lastSlashPt == std::string::npos) {
        return "";
    }

    std::string binDirPath = binFilePath.substr(0, lastSlashPt);
    return binDirPath;
}

bool createDir(const std::string& dirName) {
    bool res = false;
#ifdef PLATFORM_WINDOWS
    res = CreateDirectory(dirName.c_str(), nullptr);
#else
    res = !mkdir(dirName.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
#endif
    return res;
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

void normalizePath(std::string& origPath) {
    bool isStartFromSlash = !origPath.empty() && (origPath[0] == VALID_SLASH);
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
    if(isStartFromSlash) {
        origPath += VALID_SLASH;
    }
    for(const auto& token : resPath) {
        if(!origPath.empty()) {
            origPath += VALID_SLASH;
        }
        origPath += token;
    }
}

std::string getAssetDirPath() {
    std::string binDirPath = getBinDir();
    if(binDirPath.empty()) {
        return "";
    }
    std::vector<std::string> possiblePaths = {
        "/Assets",
        "/../Assets",
        "/../../Assets",
        "/../../../../Assets",
    };
    for(const auto& path : possiblePaths) {
        auto dirPath = binDirPath + path;
        fixSlashes(dirPath);
        normalizePath(dirPath);
        if(isDirExist(dirPath)) {
            return dirPath;
        }
    }
    return "";
}

std::string getLoacalFilesDirPath() {
    std::string binDirPath = getBinDir();
    if(binDirPath.empty()) {
        return "";
    }
    fixSlashes(binDirPath);
    std::string dirPath = StringFormat("%s%c%s", binDirPath, VALID_SLASH, LOCAL_DIR_PATH);
    if(!isDirExist(dirPath)) {
        if(!createDir(dirPath)) {
            return "";
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

std::string GetSafeStrErrno() {
#ifdef PLATFORM_WINDOWS
    char cErrorMsg[MAX_ERROR_MSG_LEN];
    strerror_s(&(cErrorMsg[0]), MAX_ERROR_MSG_LEN, errno);
    return &(cErrorMsg[0]); 
#else
    return strerror(errno);
#endif
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
    if(buff) {
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
        LogError("[DesktopAssets::ET_saveLocalFile] Can't write data to a dir object: '%s'", filePath);
        return false;
    }
    std::ofstream fout(filePath, std::ios::out | std::ios::binary);
    if(!fout.good() || !fout.is_open()) {
        LogError("[DesktopAssets::ET_saveLocalFile] Can't create/open a file: '%s'; Error: %s", filePath, GetSafeStrErrno());
        return false;
    }
    if(!fout.write(static_cast<const char*>(buff.getReadData()), buff.getSize())) {
        LogError("[DesktopAssets::ET_saveLocalFile] Can't write to the file: '%s'; Error: %s", filePath, GetSafeStrErrno());
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
        LogError("[DesktopAssets::ET_removeLocalFile] Can't remove a dir object: '%s'", filePath);
        return false;
    }
    if(remove(filePath.c_str())) {
        LogError("[DesktopAssets::ET_removeLocalFile] Unable to remove file '%s'; Error: %s", filePath, GetSafeStrErrno());
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
    if(isDirExist(filePath)) {
        LogError("[DesktopAssets::loadFileFromDirImpl] Can't opend dir as file: '%s'", filePath);
        return Buffer();
    }
    std::ifstream fin(filePath, std::ios::binary | std::ios::ate);
    if(!fin.good() || !fin.is_open()) {
        LogError("[DesktopAssets::loadFileFromDir] Can't load file: '%s'. Error: %s", filePath, GetSafeStrErrno());
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