// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "sysfswriter.h"

#include "common/fileutils.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <exception>
#include <ostream>

void SysfsWriter::write(std::filesystem::path const &sysfsEntry,
                        std::string const &value)
{
  if (!Utils::File::isFilePathValid(sysfsEntry)) {
    LOG(ERROR) << fmt::format("Invalid file path {}", sysfsEntry.c_str());
    return;
  }

  if (!isSysfsPath(sysfsEntry)) {
    LOG(ERROR) << fmt::format(
        "{} is not a sysfs path. Value {} wont be written.", sysfsEntry.c_str(),
        value);
    return;
  }

  std::ofstream file(sysfsEntry);
  if (!file.is_open()) {
    LOG(ERROR) << fmt::format("Cannot write {} to file {}", value,
                              sysfsEntry.c_str());
    return;
  }

  file << value << std::endl;
}

bool SysfsWriter::isSysfsPath(std::filesystem::path const &path) const
{
  try {
    // check whether canonical path starts with '/sys'
    return std::filesystem::canonical(path).string().compare(
               0, sysfsPath.length(), sysfsPath) == 0;
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }
  return false;
}
