// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <filesystem>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace Utils {
namespace File {

/// Reads the contents of a file.
/// @param path path to the file to read
/// @returns file contents or no contents on error
///
/// @note Reading /sys files will produce more data than expected as
/// its file size doesn't match the actual file contents.
/// @note Reading /proc files won't produce any data. Those files must
/// be readed line by line. Use readFileLines instead.
std::vector<char> readFile(std::filesystem::path const &path);

/// Writes data to a file. Any previous file contents will be destroyed.
/// @param path path to the file to read
/// @param data to be written
/// @returns true on success
bool writeFile(std::filesystem::path const &path, std::vector<char> const &data);

/// Read the data of a file line by line. Contents will be splitted in
/// lines using a delimiter.
/// @param path file to read
/// @param delimiter new line character
/// @returns lines of data
std::vector<std::string> readFileLines(std::filesystem::path const &path,
                                       char delim = '\n');

/// Checks if a path exists and points to a file.
/// @param path file to validate
/// @returns true if the path points to a valid file
bool isFilePathValid(std::filesystem::path const &path);

/// Checks if a path exists and points to a directory.
/// @param path directory to validate
/// @returns true if the path points to a valid directory
bool isDirectoryPathValid(std::filesystem::path const &path);

/// Search for files or directories whose name matches a regular expression.
/// @param regex regular expression to match the directory entries
/// @param path target directory to search
/// @returns path to the directory entries that match the regular expression
std::vector<std::filesystem::path> search(std::regex const &regex,
                                          std::filesystem::path const &path);

/// Find the hwmon[0-*] directory in a path.
/// @param path directory to seach for hwmon[0-*] directory
/// @returns path to hwmon[0-*] directory
std::optional<std::filesystem::path>
findHWMonXDirectory(std::filesystem::path const &path);

/// Checks if a path points to an existing and non empty sysfs entry file.
/// @param path file to validate
/// @return true if the path points to a valid sysfs entry
bool isSysFSEntryValid(std::filesystem::path const &path);

/// Read file data from a Qt qrc path.
/// @param path rcc file path
/// @return file contents or no contents on error
std::vector<char> readQrcFile(std::string_view qrcPath);

} // namespace File
} // namespace Utils
