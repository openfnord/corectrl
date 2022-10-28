// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string>
#include <vector>

class ISysExplorer
{
 public:
  /// @returns list of supported render device names (format: renderD<number>)
  virtual std::vector<std::string> renderers() = 0;

  virtual ~ISysExplorer() = default;
};
