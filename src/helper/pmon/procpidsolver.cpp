// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "procpidsolver.h"

#include "common/fileutils.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <system_error>

std::string ProcPIDSolver::app(int PID)
{
  auto const procPIDPath = procPath_ / std::to_string(PID);
  auto exeName = procExeFileName(procPIDPath);

  // Handle wine apps.
  if (wineExecutables_.find(exeName) != wineExecutables_.cend()) {
    auto const entries = Utils::File::readFileLines(procPIDPath / "cmdline",
                                                    '\0');
    auto const wineExeName = wineAppName(entries);
    if (!wineExeName.empty())
      exeName = wineExeName;
  }

  return exeName;
}

std::string
ProcPIDSolver::procExeFileName(std::filesystem::path const &procPIDPath) const
{
  std::error_code ec;
  return std::filesystem::read_symlink(procPIDPath / "exe", ec).filename();
}

std::string ProcPIDSolver::lastComponent(std::string const &path) const
{
  size_t const pos = path.find_last_of("\\/");
  if (pos == std::string::npos)
    return path;

  return path.substr(pos + 1);
}

std::string ProcPIDSolver::wineAppName(std::vector<std::string> const &cmdline) const
{
  for (auto &entry : cmdline) {

    // skip wine executable files
    std::filesystem::path const entryPath(entry);
    if (entryPath.is_absolute() &&
        wineExecutables_.find(entryPath.filename()) != wineExecutables_.cend())
      continue;

    // look for .exe extension
    std::string extension = entryPath.extension();
    std::transform(extension.cbegin(), extension.cend(), extension.begin(),
                   ::tolower);
    if (extension != ".exe")
      break;

    return lastComponent(entry);
  }

  return "";
}
