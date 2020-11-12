#include "FileUtils.hpp"
#include "Core/ETLogger.hpp"

#include <fstream>

#ifdef PLATFORM_WINDOWS
  #include <direct.h>
  #include <windows.h>
#elif defined PLATFORM_LINUX
  #include <unistd.h>
#elif defined PLATFORM_ANDROID
  #include <unistd.h>
#else
  #error build platfrom macro not specified
#endif

namespace {

const size_t MAX_ERROR_MSG_LEN = 128;

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

namespace FileUtils {

std::filesystem::path CombinePath(const std::filesystem::path& dirName, const char* fileName) {
    std::filesystem::path resPath = dirName;
    resPath.append(fileName);
    resPath = resPath.make_preferred();
    return resPath;
}

bool IsFileExists(const std::filesystem::path& path) {
    std::error_code errCode;
    if(!std::filesystem::exists(path, errCode)) {
        if(errCode) {
            LogError("[IsFileExists] Can't query dir status: '%s' (Error: %s)", path.string(), errCode.message());
        }
        return false;
    }
    errCode.clear();
    if(std::filesystem::is_directory(path, errCode)) {
        if(errCode) {
            LogError("[IsFileExists] Can't query file info: '%s' (Error: %s)", path.string(), errCode.message());
        }
        return false;
    }
    return true;
}

bool IsDirExists(const std::filesystem::path& path) {
    std::error_code errCode;
    if(!std::filesystem::exists(path, errCode)) {
        if(errCode) {
            LogError("[IsDirExists] Can't query dir status: '%s' (Error: %s)", path.string(), errCode.message());
        }
        return false;
    }
    errCode.clear();
    if(!std::filesystem::is_directory(path, errCode)) {
        if(errCode) {
            LogError("[IsDirExists] Can't query file info: '%s' (Error: %s)", path.string(), errCode.message());
        }
        return false;
    }
    return true;
}

bool CreateDir(const std::filesystem::path& path) {
    std::error_code errCode;
    if(!std::filesystem::create_directory(path, errCode)) {
        if(errCode) {
            LogError("[CreateDir] Can't create directory: '%s' (Error: %s)", path.string(), errCode.message());
        } else {
            LogError("[CreateDir] Can't create directory: '%s' (Error: Unknown)", path.string());
        }
        return false;
    }
    return true;
}

std::string NormilizeAssetName(const char* assetName) {
    if(!assetName || !assetName[0]) {
        return std::string();
    }
    std::string name = assetName;
    for(auto& ch : name) {
        if(ch == '\\') {
            ch = '/';
        }
    }
    if(name[0] == '/') {
        name.assign(name.substr(1, name.size()-1));
    }
    return name;
}

Buffer LoadFile(const std::filesystem::path& fileName) {
    if(IsDirExists(fileName)) {
        LogError("[LoadFile] Can't opend dir as file: '%s'", fileName.string());
        return Buffer();
    }
    std::ifstream fin(fileName, std::ios::binary | std::ios::ate);
    if(!fin.good() || !fin.is_open()) {
        LogError("[LoadFile] Can't open file: '%s' (Error: %s)", fileName.string(), GetSafeStrErrno());
        return Buffer();
    }
    std::streamoff fileSize = fin.tellg();
    if(fileSize == -1) {
        LogError("[LoadFile] Can't get file size: '%s'", fileName.string());
        return Buffer();
    }
    fin.seekg(0u, std::ios::beg);
    Buffer buffer(fileSize);
    if(!buffer) {
        LogError("[LoadFile] Can't allocate buffer of size %ld bytes to load file: '%s'", fileSize, fileName.string());
        return Buffer();
    }
    if(!fin.read(static_cast<char*>(buffer.getWriteData()), fileSize)) {
        LogError("[LoadFile] Can't read from file: '%s'", fileName.string());
        return Buffer();
    }
    return buffer;
}

Buffer LoadFileFromDir(const std::filesystem::path& dirName, const char* fileName) {
    auto filePath = CombinePath(dirName, fileName);
    return LoadFile(filePath);
}

bool RemoveFileFromDir(const std::filesystem::path& dirName, const char* fileName) {
    auto filePath = CombinePath(dirName, fileName);
    if(filePath.empty()) {
        LogError("[RemoveFileFromDir] Can't remove a file by invalid name: '%s'", fileName);
        return false;
    }
    if(FileUtils::IsDirExists(filePath)) {
        LogError("[RemoveFileFromDir] Can't remove a dir object: '%s'", filePath.string());
        return false;
    }
    std::error_code errCode;
    if(!std::filesystem::remove(filePath, errCode)) {
        if(errCode) {
            LogError("[RemoveFileFromDir] Unable to remove file '%s' (Error: %s)", filePath.string(), errCode.message());
        } else {
            LogError("[RemoveFileFromDir] Unable to remove file '%s' (Error: Unknown)", filePath.string());
        }
        return false;
    }
    return true;
}

bool SaveFileToDir(const std::filesystem::path& dirName, const char* fileName, const Buffer& buffer) {
    auto filePath = CombinePath(dirName, fileName);
    if(filePath.empty()) {
        LogError("[SaveFileToDir] Can't save a file by invalid name: '%s'", fileName);
        return false;
    }
    if(IsDirExists(filePath)) {
        LogError("[SaveFileToDir] Can't write data to a dir object: '%s'", filePath.string());
        return false;
    }
    std::ofstream fout(filePath, std::ios::out | std::ios::binary);
    if(!fout.good() || !fout.is_open()) {
        LogError("[SaveFileToDir] Can't create/open a file: '%s' (Error: %s)", filePath.string(), GetSafeStrErrno());
        return false;
    }
    if(!fout.write(static_cast<const char*>(buffer.getReadData()), buffer.getSize())) {
        LogError("[SaveFileToDir] Can't write to the file: '%s' (Error: %s)", filePath.string(), GetSafeStrErrno());
        return false;
    }
    return true;
}

} // namespace FileUtils