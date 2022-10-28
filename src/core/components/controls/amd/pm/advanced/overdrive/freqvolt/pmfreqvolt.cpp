// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqvolt.h"

#include "core/components/amdutils.h"
#include "core/components/controls/amd/pm/handlers/ippdpmhandler.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <iterator>

AMD::PMFreqVolt::PMFreqVolt(
    std::string &&controlName, std::string &&controlCmdId,
    std::unique_ptr<IDataSource<std::vector<std::string>>> &&ppOdClkVoltDataSource,
    std::unique_ptr<IPpDpmHandler> &&ppDpmHandler) noexcept
: Control(true)
, id_(AMD::PMFreqVolt::ItemID)
, controlName_(std::move(controlName))
, controlCmdId_(std::move(controlCmdId))
, ppDpmHandler_(std::move(ppDpmHandler))
, ppOdClkVoltDataSource_(std::move(ppOdClkVoltDataSource))
, voltModes_({"auto", "manual"})
, voltMode_(AMD::PMFreqVolt::VoltMode::Automatic)
{
}

void AMD::PMFreqVolt::preInit(ICommandQueue &ctlCmds)
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {
    preInitStates_ = Utils::AMD::parseOverdriveClksVolts(controlName(),
                                                         ppOdClkVoltLines_)
                         .value();
    ppDpmHandler_->saveState();
    cleanControl(ctlCmds);
  }
}

void AMD::PMFreqVolt::postInit(ICommandQueue &ctlCmds)
{
  for (auto [index, freq, volt] : preInitStates_) {
    ctlCmds.add(
        {ppOdClkVoltDataSource_->source(), ppOdClkVoltCmd(index, freq, volt)});
  }

  if (!preInitStates_.empty())
    ppDpmHandler_->restoreState(ctlCmds);
}

void AMD::PMFreqVolt::init()
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {

    freqRange_ = Utils::AMD::parseOverdriveClkRange(controlName(),
                                                    ppOdClkVoltLines_)
                     .value();
    voltRange_ = Utils::AMD::parseOverdriveVoltRange(ppOdClkVoltLines_).value();

    auto states = Utils::AMD::parseOverdriveClksVolts(controlName(),
                                                      ppOdClkVoltLines_);
    auto [voltMin, voltMax] = voltRange_;
    auto [freqMin, freqMax] = freqRange_;
    for (auto [index, freq, volt] : states.value()) {
      initVoltages_.emplace(index, volt);
      states_.emplace(index, std::make_pair(std::clamp(freq, freqMin, freqMax),
                                            std::clamp(volt, voltMin, voltMax)));
    }
  }
}

std::string const &AMD::PMFreqVolt::ID() const
{
  return id_;
}

std::string const &AMD::PMFreqVolt::instanceID() const
{
  return controlName();
}

void AMD::PMFreqVolt::importControl(IControl::Importer &i)
{
  auto &importer = dynamic_cast<AMD::PMFreqVolt::Importer &>(i);

  voltMode(importer.providePMFreqVoltVoltMode());

  for (auto [index, _] : states_) {
    auto [freq, volt] = importer.providePMFreqVoltState(index);
    state(index, freq, volt);
  }

  ppDpmHandler_->activate(importer.providePMFreqVoltActiveStates());
}

void AMD::PMFreqVolt::exportControl(IControl::Exporter &e) const
{
  auto &exporter = dynamic_cast<AMD::PMFreqVolt::Exporter &>(e);

  exporter.takePMFreqVoltControlName(controlName());

  exporter.takePMFreqVoltVoltModes(voltModes());
  exporter.takePMFreqVoltVoltMode(voltMode());

  auto [freqMim, freqMax] = freqRange();
  exporter.takePMFreqVoltFreqRange(freqMim, freqMax);

  auto [voltMim, voltMax] = voltRange();
  exporter.takePMFreqVoltVoltRange(voltMim, voltMax);
  exporter.takePMFreqVoltStates(states());

  exporter.takePMFreqVoltActiveStates(ppDpmHandler_->active());
}

void AMD::PMFreqVolt::cleanControl(ICommandQueue &ctlCmds)
{
  ppDpmHandler_->reset(ctlCmds);
}

void AMD::PMFreqVolt::syncControl(ICommandQueue &ctlCmds)
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {

    auto states = Utils::AMD::parseOverdriveClksVolts(controlName(),
                                                      ppOdClkVoltLines_);
    for (auto [index, freq, volt] : states.value()) {
      auto [targetFreq, sVolt] = states_.at(index);
      auto targetVolt = voltMode_ == AMD::PMFreqVolt::VoltMode::Automatic
                            ? initVoltages_.at(index)
                            : sVolt;
      if (freq != targetFreq || volt != targetVolt) {
        ctlCmds.add({ppOdClkVoltDataSource_->source(),
                     ppOdClkVoltCmd(index, targetFreq, targetVolt)});
      }
    }

    ppDpmHandler_->sync(ctlCmds);
  }
}

std::string const &AMD::PMFreqVolt::controlName() const
{
  return controlName_;
}

std::string const &AMD::PMFreqVolt::controlCmdId() const
{
  return controlCmdId_;
}

std::vector<std::string> const &AMD::PMFreqVolt::voltModes() const
{
  return voltModes_;
}

void AMD::PMFreqVolt::voltMode(std::string const &mode)
{
  voltMode_ = mode == voltModes_[0] ? AMD::PMFreqVolt::VoltMode::Automatic
                                    : AMD::PMFreqVolt::VoltMode::Manual;
}

std::string const &AMD::PMFreqVolt::voltMode() const
{
  return voltModes_[static_cast<int>(voltMode_)];
}

std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
AMD::PMFreqVolt::freqRange() const
{
  return freqRange_;
}

std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> const &
AMD::PMFreqVolt::voltRange() const
{
  return voltRange_;
}

std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                       units::voltage::millivolt_t>>
AMD::PMFreqVolt::states() const
{
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      states;
  states.reserve(states_.size());
  std::transform(
      states_.cbegin(), states_.cend(), std::back_inserter(states), [](auto &kv) {
        return std::make_tuple(kv.first, kv.second.first, kv.second.second);
      });
  return states;
}

void AMD::PMFreqVolt::state(unsigned int index,
                            units::frequency::megahertz_t freq,
                            units::voltage::millivolt_t volt)
{
  auto [freqMin, freqMax] = freqRange();
  auto [voltMin, voltMax] = voltRange();
  auto &[sFreq, sVolt] = states_.at(index);
  sFreq = std::clamp(freq, freqMin, freqMax);
  sVolt = std::clamp(volt, voltMin, voltMax);
}

std::string AMD::PMFreqVolt::ppOdClkVoltCmd(unsigned int index,
                                            units::frequency::megahertz_t freq,
                                            units::voltage::millivolt_t volt) const
{
  std::string cmd;
  cmd.reserve(16);
  cmd.append(controlCmdId())
      .append(" ")
      .append(std::to_string(index))
      .append(" ")
      .append(std::to_string(freq.to<unsigned int>()))
      .append(" ")
      .append(std::to_string(volt.to<unsigned int>()));
  return cmd;
}
