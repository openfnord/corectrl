// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string>

class IPIDSolver
{
 public:
  /// Translates a PID to its application executable name
  /// @param PID application's PID
  /// @return application executable name
  virtual std::string app(int PID) = 0;

  virtual ~IPIDSolver() = default;
};
