// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "ippdpmhandler.h"
#include "units/units.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class PpDpmHandler : public IPpDpmHandler
{
 public:
  PpDpmHandler(std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
               std::unique_ptr<IDataSource<std::vector<std::string>>>
                   &&ppDpmDataSource) noexcept;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &
  states() const override;

  std::vector<unsigned int> const &active() const override;
  void activate(std::vector<unsigned int> const &states) override;

  void saveState() override;
  void restoreState(ICommandQueue &ctlCmds) override;

  void reset(ICommandQueue &ctlCmds) override;
  void sync(ICommandQueue &ctlCmds) override;

 private:
  void apply(ICommandQueue &ctlCmds);

  std::unique_ptr<IDataSource<std::string>> const perfLevelDataSource_;
  std::unique_ptr<IDataSource<std::vector<std::string>>> const ppDpmDataSource_;

  std::string perfLevelValue_;
  std::vector<std::string> ppDpmLines_;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states_;
  std::vector<unsigned int> active_;

  bool resync_;
};

} // namespace AMD
