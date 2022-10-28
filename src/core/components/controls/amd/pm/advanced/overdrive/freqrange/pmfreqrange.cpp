// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqrange.h"

#include "core/components/amdutils.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <cstddef>
#include <iterator>

AMD::PMFreqRange::PMFreqRange(
    std::string &&controlName, std::string &&controlCmdId,
    std::unique_ptr<IDataSource<std::vector<std::string>>> &&ppOdClkVoltDataSource,
    std::optional<DisabledBound> &&disabledBound) noexcept
: Control(true)
, id_(AMD::PMFreqRange::ItemID)
, controlName_(std::move(controlName))
, controlCmdId_(std::move(controlCmdId))
, disabledBound_(std::move(disabledBound))
, ppOdClkVoltDataSource_(std::move(ppOdClkVoltDataSource))
{
}

void AMD::PMFreqRange::preInit(ICommandQueue &)
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {
    preInitStates_ =
        Utils::AMD::parseOverdriveClks(controlName(), ppOdClkVoltLines_).value();
  }
}

void AMD::PMFreqRange::postInit(ICommandQueue &ctlCmds)
{
  for (auto [index, freq] : preInitStates_) {
    // skip disabled bound
    if (disabledBound_.has_value() && index == disabledBound_->index)
      continue;

    ctlCmds.add({ppOdClkVoltDataSource_->source(), ppOdClkVoltCmd(index, freq)});
  }
}

void AMD::PMFreqRange::init()
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {
    stateRange_ = Utils::AMD::parseOverdriveClkRange(controlName(),
                                                     ppOdClkVoltLines_)
                      .value();
    auto states = Utils::AMD::parseOverdriveClks(controlName(),
                                                 ppOdClkVoltLines_);
    auto [min, max] = stateRange_;
    for (auto [index, freq] : states.value()) {
      // skip disabled bound
      if (disabledBound_.has_value() && index == disabledBound_->index)
        continue;

      states_.emplace(index, (std::clamp(freq, min, max)));
    }
  }
}

std::string const &AMD::PMFreqRange::ID() const
{
  return id_;
}

std::string const &AMD::PMFreqRange::instanceID() const
{
  return controlName();
}

void AMD::PMFreqRange::importControl(IControl::Importer &i)
{
  auto &importer = dynamic_cast<AMD::PMFreqRange::Importer &>(i);
  for (auto [index, _] : states_)
    state(index, importer.providePMFreqRangeState(index));
}

void AMD::PMFreqRange::exportControl(IControl::Exporter &e) const
{
  auto &exporter = dynamic_cast<AMD::PMFreqRange::Exporter &>(e);

  auto [mim, max] = stateRange();
  exporter.takePMFreqRangeControlName(controlName());
  exporter.takePMFreqRangeStateRange(mim, max);
  exporter.takePMFreqRangeStates(states());
}

void AMD::PMFreqRange::cleanControl(ICommandQueue &)
{
}

void AMD::PMFreqRange::syncControl(ICommandQueue &ctlCmds)
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {

    auto states = Utils::AMD::parseOverdriveClks(controlName(),
                                                 ppOdClkVoltLines_);
    for (auto [index, freq] : states.value()) {
      // skip disabled bound
      if (disabledBound_.has_value() && index == disabledBound_->index)
        continue;

      auto targetFreq = states_.at(index);
      if (freq != targetFreq) {
        ctlCmds.add({ppOdClkVoltDataSource_->source(),
                     ppOdClkVoltCmd(index, targetFreq)});
      }
    }
  }
}

std::string const &AMD::PMFreqRange::controlName() const
{
  return controlName_;
}

std::string const &AMD::PMFreqRange::controlCmdId() const
{
  return controlCmdId_;
}

std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
AMD::PMFreqRange::stateRange() const
{
  return stateRange_;
}

std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>
AMD::PMFreqRange::states() const
{
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states;
  states.reserve(states_.size());
  std::transform(states_.cbegin(), states_.cend(), std::back_inserter(states),
                 [](auto &kv) { return std::make_pair(kv.first, kv.second); });
  return states;
}

void AMD::PMFreqRange::state(unsigned int index,
                             units::frequency::megahertz_t freq)
{
  auto [min, max] = stateRange();
  auto &sFreq = states_.at(index);
  sFreq = std::clamp(freq, min, max);
}

std::string AMD::PMFreqRange::ppOdClkVoltCmd(unsigned int index,
                                             units::frequency::megahertz_t freq) const
{
  std::string cmd;
  cmd.reserve(16);
  cmd.append(controlCmdId())
      .append(" ")
      .append(std::to_string(index))
      .append(" ")
      .append(std::to_string(freq.to<unsigned int>()));
  return cmd;
}
