//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
  // Those apps are launched using wine[64]-preloader.
  if (winePreloaders_.find(exeName) != winePreloaders_.cend()) {
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
