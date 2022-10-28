// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fileutils.h"

#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <QFile>
#include <exception>
#include <fstream>
#include <utility>

namespace fs = std::filesystem;

namespace Utils {
namespace File {

std::vector<char> readFile(std::filesystem::path const &path)
{
  std::vector<char> data;

  if (isFilePathValid(path)) {
    std::ifstream file(path, std::ios::binary);
    if (file.is_open()) {
      data.resize(fs::file_size(path));
      file.read(data.data(), data.size());
    }
    else
      LOG(ERROR) << fmt::format("Cannot open file {}", path.c_str());
  }
  else
    LOG(ERROR) << fmt::format("Invalid file path {}", path.c_str());

  return data;
}

bool writeFile(std::filesystem::path const &path, std::vector<char> const &data)
{
  std::ofstream file(path, std::ios::binary);
  if (file.is_open()) {
    file.write(data.data(), data.size());
    return true;
  }
  else
    LOG(ERROR) << fmt::format("Cannot open file {}", path.c_str());

  return false;
}

std::vector<std::string> readFileLines(std::filesystem::path const &path,
                                       char delim)
{
  std::vector<std::string> entries;

  if (isFilePathValid(path)) {
    std::ifstream file(path);
    if (file.is_open()) {
      for (std::string entry; std::getline(file, entry, delim);)
        entries.emplace_back(std::move(entry));
    }
    else
      LOG(ERROR) << fmt::format("Cannot open file {}", path.c_str());
  }
  else
    LOG(ERROR) << fmt::format("Invalid file path {}", path.c_str());

  return entries;
}

bool isFilePathValid(std::filesystem::path const &path)
{
  try {
    return fs::exists(path) && fs::is_regular_file(path);
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }

  return false;
}

bool isDirectoryPathValid(std::filesystem::path const &path)
{
  try {
    return fs::exists(path) && fs::is_directory(path);
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }

  return false;
}

std::vector<std::filesystem::path> search(std::regex const &regex,
                                          std::filesystem::path const &path)
{
  std::vector<std::filesystem::path> paths;

  if (isDirectoryPathValid(path)) {
    for (auto &entry : fs::directory_iterator(path)) {
      auto &entryDirPath = entry.path();
      std::string entryDirName((*(--entryDirPath.end())).string());
      if (std::regex_search(entryDirName, regex))
        paths.push_back(entryDirPath);
    }
  }
  else
    LOG(ERROR) << fmt::format("Invalid directory path {}", path.c_str());

  return paths;
}

std::optional<std::filesystem::path>
findHWMonXDirectory(std::filesystem::path const &path)
{
  std::regex const hwmonXRegex(R"(hwmon[0-9]+)");
  auto paths = Utils::File::search(hwmonXRegex, path);
  if (!paths.empty()) {
    if (paths.size() > 1) {
      LOG(WARNING) << fmt::format(
          "Multiple hwmon directories detected on {}.\nUsing {}", path.c_str(),
          paths.front().c_str());
    }

    // use the first hwmon[0-*] directory from gpu sysfs/hwmon path
    return paths.front();
  }

  return {};
}

bool isSysFSEntryValid(std::filesystem::path const &path)
{
  if (!isFilePathValid(path))
    return false;

  if (readFileLines(path).empty()) {
    LOG(WARNING) << fmt::format("Empty sysfs entry {}", path.c_str());
    return false;
  }

  return true;
}

std::vector<char> readQrcFile(std::string_view qrcPath)
{
  QFile file(qrcPath.data());
  if (file.open(QFile::ReadOnly)) {
    auto fileData = file.readAll();
    return std::vector<char>(fileData.cbegin(), fileData.cend());
  }

  return {};
}

} // namespace File
} // namespace Utils
