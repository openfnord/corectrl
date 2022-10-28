// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include "units/units.h"
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class IPpDpmHandler;

/// Overdrive frequency + voltage control.
///
/// A frequency + voltage overdrive control has an identifier and a
/// list of states that operates on both frequency and voltage values.
///
/// States can be toggled on/off individually.
class PMFreqVolt : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_FREQ_VOLT"};

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &providePMFreqVoltVoltMode() const = 0;
    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMFreqVoltState(unsigned int index) const = 0;
    virtual std::vector<unsigned int> providePMFreqVoltActiveStates() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMFreqVoltControlName(std::string const &name) = 0;
    virtual void
    takePMFreqVoltVoltModes(std::vector<std::string> const &modes) = 0;
    virtual void takePMFreqVoltVoltMode(std::string const &mode) = 0;
    virtual void takePMFreqVoltFreqRange(units::frequency::megahertz_t min,
                                         units::frequency::megahertz_t max) = 0;
    virtual void takePMFreqVoltVoltRange(units::voltage::millivolt_t min,
                                         units::voltage::millivolt_t max) = 0;
    virtual void takePMFreqVoltStates(
        std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                               units::voltage::millivolt_t>> const &states) = 0;
    virtual void
    takePMFreqVoltActiveStates(std::vector<unsigned int> const &states) = 0;
  };

  PMFreqVolt(std::string &&controlName, std::string &&controlCmdId,
             std::unique_ptr<IDataSource<std::vector<std::string>>>
                 &&ppOdClkVoltDataSource,
             std::unique_ptr<IPpDpmHandler> &&ppDpmHandler) noexcept;

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

  std::vector<std::string> const &voltModes() const;
  void voltMode(std::string const &mode);
  std::string const &voltMode() const;

  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
  freqRange() const;
  std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> const &
  voltRange() const;

  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
  states() const;
  void state(unsigned int index, units::frequency::megahertz_t freq,
             units::voltage::millivolt_t volt);

  std::string ppOdClkVoltCmd(unsigned int index,
                             units::frequency::megahertz_t freq,
                             units::voltage::millivolt_t volt) const;

 private:
  std::string const id_;
  std::string const controlName_;
  std::string const controlCmdId_;

  std::unique_ptr<IPpDpmHandler> const ppDpmHandler_;
  std::unique_ptr<IDataSource<std::vector<std::string>>> const ppOdClkVoltDataSource_;
  std::vector<std::string> ppOdClkVoltLines_;

  enum class VoltMode { Automatic, Manual };
  std::vector<std::string> const voltModes_;
  VoltMode voltMode_;
  std::unordered_map<unsigned int, units::voltage::millivolt_t> initVoltages_;

  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      preInitStates_;
  std::map<unsigned int,
           std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      states_;

  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> freqRange_;
  std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> voltRange_;
};

} // namespace AMD
