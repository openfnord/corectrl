// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

template<typename...>
class IDataSource;

class CPUFreq : public Control
{
 public:
  static constexpr std::string_view ItemID{"CPU_CPUFREQ"};

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &provideCPUFreqScalingGovernor() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takeCPUFreqScalingGovernor(std::string const &governor) = 0;
    virtual void
    takeCPUFreqScalingGovernors(std::vector<std::string> const &governors) = 0;
  };

  CPUFreq(std::vector<std::string> &&scalingGovernors,
          std::string const &defaultGovernor,
          std::vector<std::unique_ptr<IDataSource<std::string>>>
              &&scalingGovernorDataSources) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  std::string const &scalingGovernor() const;
  void scalingGovernor(std::string const &governor);

  std::vector<std::string> const &scalingGovernors() const;

 private:
  std::string const id_;
  std::vector<std::string> const scalingGovernors_;
  std::vector<std::unique_ptr<IDataSource<std::string>>> const
      scalingGovernorDataSources_;

  std::string scalingGovernor_;
  std::string dataSourceEntry_;
};
