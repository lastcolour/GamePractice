#ifndef __FILE_UTILS_HPP__
#define __FILE_UTILS_HPP__

#include "Core/Buffer.hpp"

#include <filesystem>
#include <string>

namespace FileUtils {

bool IsFileExists(const std::filesystem::path& fileName);

bool IsDirExists(const std::filesystem::path& dirName);

bool CreateDir(const std::filesystem::path& dirName);

std::string NormilizeAssetName(const char* assetName);

Buffer LoadFileFromDir(const std::filesystem::path& dirName, const char* fileName);

Buffer LoadFile(const std::filesystem::path& fileName);

bool RemoveFileFromDir(const std::filesystem::path& dirName, const char* fileName);

bool SaveFileToDir(const std::filesystem::path& dirName, const char* fileName, const Buffer& buffer);

std::filesystem::path CombinePath(const std::filesystem::path& dirName, const char* fileName);

} // namespace FileUtils

#endif /* __FILE_UTILS_HPP__ */