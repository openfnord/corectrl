// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/idatasourcehandler.h"
#include "units/units.h"
#include <utility>
#include <vector>

namespace AMD {

class IPpDpmHandler : public IDataSourceHandler
{
 public:
  virtual std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &
  states() const = 0;

  virtual std::vector<unsigned int> const &active() const = 0;
  virtual void activate(std::vector<unsigned int> const &states) = 0;

  virtual ~IPpDpmHandler() = default;
};

} // namespace AMD
