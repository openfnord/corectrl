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
#pragma once

#include "ipidsolver.h"
#include <filesystem>
#include <set>
#include <string>
#include <vector>

class ProcPIDSolver final : public IPIDSolver
{
 public:
  std::string app(int PID) override;

 private:
  /// Gets the canonical path of the file procPIDPath/exe
  /// @param procPIDPath the process /proc/PID path
  /// @return canonical path of the file or empty string if procPIDPath/exe
  /// is not a valid path
  std::string procExeFileName(std::filesystem::path const &procPIDPath) const;

  /// Gets the executable name from path argument
  /// @param path canonical path to process' executable
  /// @param dirSeparator character used as directory separator for the path
  /// @return executable name or the empty string when path does not contains
  /// any dirSeparator character
  std::string exeFileName(std::string const &path, char const dirSeparator) const;

  /// Gets the executable name (.exe) from a wine launch command line
  /// @param cmdline /proc/PID/cmline file entries
  /// @return executable name or empty string when the command line is not a valid wine
  /// executable launch command line
  std::string wineAppName(std::vector<std::string> const &cmdline) const;

  std::filesystem::path const procPath_{"/proc"};
  std::set<std::string> const winePreloaders_{"wine-preloader",
                                              "wine64-preloader"};
  std::set<std::string> const wineExecutables_{
      "wine-preloader", "wine64-preloader", "wine",
      "wine64",         "wineloader",       "wineloader64"};
};
