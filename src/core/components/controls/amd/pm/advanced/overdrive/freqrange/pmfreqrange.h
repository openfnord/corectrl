// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include "units/units.h"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

/// Overdrive frequency range control.
///
/// A frequency overdrive control has an identifier and up to two
/// states for the minimum (optional) and maximum operation frequency
/// values.
class PMFreqRange : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_FREQ_RANGE"};

  /// Disabled range bound.
  ///
  /// When PMFreqRange is created with a disabled range bound, it won't generate
  /// any control commands for the hardware state index defined by the
  /// DisabledBound.
  struct DisabledBound
  {
    /// Hardware state index of the disabled bound.
    unsigned int index;
  };

  class Importer : public IControl::Importer
  {
   public:
    virtual units::frequency::megahertz_t
    providePMFreqRangeState(unsigned int index) const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMFreqRangeControlName(std::string const &name) = 0;
    virtual void takePMFreqRangeStateRange(units::frequency::megahertz_t min,
                                           units::frequency::megahertz_t max) = 0;
    virtual void takePMFreqRangeStates(
        std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
            &states) = 0;
  };

  PMFreqRange(
      std::string &&controlName, std::string &&controlCmdId,
      std::unique_ptr<IDataSource<std::vector<std::string>>> &&ppOdClkVoltDataSource,
      std::optional<DisabledBound> &&disabledBound = std::nullopt) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;
  std::string const &instanceID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) override;
  void syncControl(ICommandQueue &ctlCmds) override;

  std::string const &controlName() const;
  std::string const &controlCmdId() const;

  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
  stateRange() const;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>
  states() const;

  void state(unsigned int index, units::frequency::megahertz_t freq);

  std::string ppOdClkVoltCmd(unsigned int index,
                             units::frequency::megahertz_t freq) const;

 private:
  std::string const id_;
  std::string const controlName_;
  std::string const controlCmdId_;
  std::optional<DisabledBound> const disabledBound_;

  std::unique_ptr<IDataSource<std::vector<std::string>>> const ppOdClkVoltDataSource_;
  std::vector<std::string> ppOdClkVoltLines_;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> preInitStates_;
  std::map<unsigned int, units::frequency::megahertz_t> states_;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> stateRange_;
};

} // namespace AMD
