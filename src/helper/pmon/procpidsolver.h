// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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

  /// Returns the last component of a windows or unix path
  std::string lastComponent(std::string const &path) const;

  /// Gets the executable name (.exe) from a wine launch command line
  /// @param cmdline /proc/PID/cmline file entries
  /// @return executable name or empty string when the command line is not a valid wine
  /// executable launch command line
  std::string wineAppName(std::vector<std::string> const &cmdline) const;

  std::filesystem::path const procPath_{"/proc"};
  std::set<std::string> const wineExecutables_{
      "wine-preloader", "wine64-preloader", "wine",
      "wine64",         "wineloader",       "wineloader64"};
};
