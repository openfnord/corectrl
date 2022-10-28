// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class PMPowerProfile : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_POWER_PROFILE"};

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &providePMPowerProfileMode() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMPowerProfileMode(std::string const &mode) = 0;
    virtual void
    takePMPowerProfileModes(std::vector<std::string> const &modes) = 0;
  };

  PMPowerProfile(std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
                 std::unique_ptr<IDataSource<std::vector<std::string>>>
                     &&powerProfileDataSource,
                 std::vector<std::pair<std::string, int>> const &modes) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  void mode(std::string const &mode);
  std::string const &mode() const;
  std::vector<std::string> const &modes() const;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<std::string>> const perfLevelDataSource_;
  std::unique_ptr<IDataSource<std::vector<std::string>>> const powerProfileDataSource_;

  int currentModeIndex_;
  int defaultModeIndex_;
  std::vector<std::string> modes_;
  std::unordered_map<int, std::string> indexMode_;

  std::string dataSourceEntry_;
  std::vector<std::string> dataSourceLines_;
};

} // namespace AMD
