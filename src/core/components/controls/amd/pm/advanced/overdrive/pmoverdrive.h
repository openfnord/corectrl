// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlgroup.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class PMOverdrive : public ControlGroup
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_OVERDRIVE"};

  // Legacy overdrive controls ItemIDs
  static constexpr std::string_view LegacyFVStateItemID{"AMD_PM_FV_STATE"};
  static constexpr std::string_view LegacyFVVoltCurveItemID{
      "AMD_PM_FV_VOLTCURVE"};

  PMOverdrive(std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
              std::unique_ptr<IDataSource<std::vector<std::string>>>
                  &&ppOdClkVoltDataSource,
              std::vector<std::unique_ptr<IControl>> &&controls) noexcept;

  void preInit(ICommandQueue &ctlCmds) override;
  void postInit(ICommandQueue &ctlCmds) override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

 protected:
  std::string const &perfLevelPreInitValue() const;

 private:
  std::unique_ptr<IDataSource<std::string>> const perfLevelDataSource_;
  std::unique_ptr<IDataSource<std::vector<std::string>>> const ppOdClkVoltDataSource_;

  std::string perfLevelEntry_;
  std::string perfLevelPreInitValue_;
};

} // namespace AMD
