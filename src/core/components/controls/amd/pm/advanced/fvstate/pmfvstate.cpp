//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
#include "pmfvstate.h"

#include "core/components/amdutils.h"
#include "core/components/controls/amd/pm/handlers/ippdpmhandler.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <iterator>

AMD::PMFVState::PMFVState(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
    std::unique_ptr<IDataSource<std::vector<std::string>>> &&ppOdClkVoltDataSource,
    std::unique_ptr<IPpDpmHandler> &&ppDpmSclkHandler,
    std::unique_ptr<IPpDpmHandler> &&ppDpmMclkHandler) noexcept
: Control(true)
, id_(AMD::PMFVState::ItemID)
, perfLevelDataSource_(std::move(perfLevelDataSource))
, ppOdClkVoltDataSource_(std::move(ppOdClkVoltDataSource))
, ppDpmSclkHandler_(std::move(ppDpmSclkHandler))
, ppDpmMclkHandler_(std::move(ppDpmMclkHandler))
, voltModes_({"auto", "manual"})
, gpuVoltMode_(AMD::PMFVState::VoltMode::Automatic)
, memVoltMode_(AMD::PMFVState::VoltMode::Automatic)
{
}

void AMD::PMFVState::preInit(ICommandQueue &ctlCmds)
{
  perfLevelDataSource_->read(perfLevelPreInitValue_);

  ppOdClkVoltDataSource_->read(ppOdClkVoltLines_);
  gpuPreInitStates_ =
      Utils::AMD::parseOdClkVoltStateStates("SCLK", ppOdClkVoltLines_).value();
  memPreInitStates_ =
      Utils::AMD::parseOdClkVoltStateStates("MCLK", ppOdClkVoltLines_).value();

  ppDpmSclkHandler_->saveState();
  ppDpmMclkHandler_->saveState();

  cleanControl(ctlCmds);
}

void AMD::PMFVState::postInit(ICommandQueue &ctlCmds)
{
  ctlCmds.add({perfLevelDataSource_->source(), perfLevelPreInitValue_});

  if (perfLevelPreInitValue_ == "manual") {

    for (auto [index, freq, volt] : gpuPreInitStates_)
      ctlCmds.add({ppOdClkVoltDataSource_->source(),
                   ppOdClkVoltStateCmd("s", index, freq, volt)});

    for (auto [index, freq, volt] : memPreInitStates_)
      ctlCmds.add({ppOdClkVoltDataSource_->source(),
                   ppOdClkVoltStateCmd("m", index, freq, volt)});

    ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});

    ppDpmSclkHandler_->restoreState(ctlCmds);
    ppDpmMclkHandler_->restoreState(ctlCmds);
  }
}

void AMD::PMFVState::init()
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {

    gpuRange_ =
        Utils::AMD::parseOdClkVoltStateClkRange("SCLK", ppOdClkVoltLines_).value();
    memRange_ =
        Utils::AMD::parseOdClkVoltStateClkRange("MCLK", ppOdClkVoltLines_).value();
    voltRange_ =
        Utils::AMD::parseOdClkVoltStateVoltRange(ppOdClkVoltLines_).value();

    auto [voltMin, voltMax] = voltRange_;

    auto gpuStates = Utils::AMD::parseOdClkVoltStateStates("SCLK",
                                                           ppOdClkVoltLines_);
    auto [gpuMin, gpuMax] = gpuRange_;
    for (auto [index, freq, volt] : gpuStates.value()) {
      gpuInitVoltages_.emplace(index, volt);
      gpuStates_.emplace(index,
                         std::make_pair(std::clamp(freq, gpuMin, gpuMax),
                                        std::clamp(volt, voltMin, voltMax)));
    }

    auto memStates = Utils::AMD::parseOdClkVoltStateStates("MCLK",
                                                           ppOdClkVoltLines_);
    auto [memMin, memMax] = memRange_;
    for (auto [index, freq, volt] : memStates.value()) {
      memInitVoltages_.emplace(index, volt);
      memStates_.emplace(index,
                         std::make_pair(std::clamp(freq, memMin, memMax),
                                        std::clamp(volt, voltMin, voltMax)));
    }
  }
}

std::string const &AMD::PMFVState::ID() const
{
  return id_;
}

void AMD::PMFVState::importControl(IControl::Importer &i)
{
  auto &pmFVStateImporter = dynamic_cast<AMD::PMFVState::Importer &>(i);

  gpuVoltMode(pmFVStateImporter.providePMFVStateGPUVoltMode());
  memVoltMode(pmFVStateImporter.providePMFVStateMemVoltMode());

  for (auto [index, _] : gpuStates_) {
    auto [freq, volt] = pmFVStateImporter.providePMFVStateGPUState(index);
    gpuState(index, freq, volt);
  }

  for (auto [index, _] : memStates_) {
    auto [freq, volt] = pmFVStateImporter.providePMFVStateMemState(index);
    memState(index, freq, volt);
  }

  ppDpmSclkHandler_->activate(
      pmFVStateImporter.providePMFVStateGPUActiveStates());
  ppDpmMclkHandler_->activate(
      pmFVStateImporter.providePMFVStateMemActiveStates());
}

void AMD::PMFVState::exportControl(IControl::Exporter &e) const
{
  auto &pmFVStateExporter = dynamic_cast<AMD::PMFVState::Exporter &>(e);

  pmFVStateExporter.takePMFVStateVoltModes(voltModes());
  pmFVStateExporter.takePMFVStateGPUVoltMode(gpuVoltMode());
  pmFVStateExporter.takePMFVStateMemVoltMode(memVoltMode());

  auto [gpuMim, gpuMax] = gpuRange();
  pmFVStateExporter.takePMFVStateGPURange(gpuMim, gpuMax);
  auto [memMim, memMax] = memRange();
  pmFVStateExporter.takePMFVStateMemRange(memMim, memMax);
  auto [voltMim, voltMax] = voltRange();
  pmFVStateExporter.takePMFVStateVoltRange(voltMim, voltMax);

  pmFVStateExporter.takePMFVStateGPUStates(gpuStates());
  pmFVStateExporter.takePMFVStateMemStates(memStates());

  pmFVStateExporter.takePMFVStateGPUActiveStates(ppDpmSclkHandler_->active());
  pmFVStateExporter.takePMFVStateMemActiveStates(ppDpmMclkHandler_->active());
}

void AMD::PMFVState::cleanControl(ICommandQueue &ctlCmds)
{
  ctlCmds.add({perfLevelDataSource_->source(), "manual"});

  ctlCmds.add({ppOdClkVoltDataSource_->source(), "r"});
  ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});

  ppDpmSclkHandler_->reset(ctlCmds);
  ppDpmMclkHandler_->reset(ctlCmds);
}

void AMD::PMFVState::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(perfLevelValue_) &&
      ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {

    if (perfLevelValue_ != "manual") {
      ctlCmds.add({perfLevelDataSource_->source(), "manual"});

      // sclk states
      for (auto [index, fv] : gpuStates_)
        ctlCmds.add({ppOdClkVoltDataSource_->source(),
                     ppOdClkVoltStateCmd("s", index, fv.first, fv.second)});

      // mclk states
      for (auto [index, fv] : memStates_)
        ctlCmds.add({ppOdClkVoltDataSource_->source(),
                     ppOdClkVoltStateCmd("m", index, fv.first, fv.second)});

      ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});

      ppDpmSclkHandler_->apply(ctlCmds);
      ppDpmMclkHandler_->apply(ctlCmds);
    }
    else {
      bool commit{false};

      auto gpuStates = Utils::AMD::parseOdClkVoltStateStates("SCLK",
                                                             ppOdClkVoltLines_);
      for (auto [index, freq, volt] : gpuStates.value()) {
        auto [targetFreq, sVolt] = gpuStates_.at(index);
        auto targetVolt = gpuVoltMode_ == AMD::PMFVState::VoltMode::Automatic
                              ? gpuInitVoltages_.at(index)
                              : sVolt;
        if (freq != targetFreq || volt != targetVolt) {
          ctlCmds.add({ppOdClkVoltDataSource_->source(),
                       ppOdClkVoltStateCmd("s", index, targetFreq, targetVolt)});
          commit = true;
        }
      }

      auto memStates = Utils::AMD::parseOdClkVoltStateStates("MCLK",
                                                             ppOdClkVoltLines_);
      for (auto [index, freq, volt] : memStates.value()) {
        auto [targetFreq, sVolt] = memStates_.at(index);
        auto targetVolt = memVoltMode_ == AMD::PMFVState::VoltMode::Automatic
                              ? memInitVoltages_.at(index)
                              : sVolt;
        if (freq != targetFreq || volt != targetVolt) {
          ctlCmds.add({ppOdClkVoltDataSource_->source(),
                       ppOdClkVoltStateCmd("m", index, targetFreq, targetVolt)});
          commit = true;
        }
      }

      if (commit)
        ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});

      ppDpmSclkHandler_->sync(ctlCmds);
      ppDpmMclkHandler_->sync(ctlCmds);
    }
  }
}

std::vector<std::string> const &AMD::PMFVState::voltModes() const
{
  return voltModes_;
}

void AMD::PMFVState::gpuVoltMode(std::string const &mode)
{
  gpuVoltMode_ = mode == voltModes_[0] ? AMD::PMFVState::VoltMode::Automatic
                                       : AMD::PMFVState::VoltMode::Manual;
}

std::string const &AMD::PMFVState::gpuVoltMode() const
{
  return voltModes_[static_cast<int>(gpuVoltMode_)];
}

void AMD::PMFVState::memVoltMode(std::string const &mode)
{
  memVoltMode_ = mode == voltModes_[0] ? AMD::PMFVState::VoltMode::Automatic
                                       : AMD::PMFVState::VoltMode::Manual;
}

std::string const &AMD::PMFVState::memVoltMode() const
{
  return voltModes_[static_cast<int>(memVoltMode_)];
}

std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
AMD::PMFVState::gpuRange() const
{
  return gpuRange_;
}

std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
AMD::PMFVState::memRange() const
{
  return memRange_;
}

std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> const &
AMD::PMFVState::voltRange() const
{
  return voltRange_;
}

std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                       units::voltage::millivolt_t>>
AMD::PMFVState::gpuStates() const
{
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      states;
  states.reserve(gpuStates_.size());
  std::transform(gpuStates_.cbegin(), gpuStates_.cend(),
                 std::back_inserter(states), [](auto &kv) {
                   return std::make_tuple(kv.first, kv.second.first,
                                          kv.second.second);
                 });
  return states;
}

std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                       units::voltage::millivolt_t>>
AMD::PMFVState::memStates() const
{
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      states;
  states.reserve(memStates_.size());
  std::transform(memStates_.cbegin(), memStates_.cend(),
                 std::back_inserter(states), [](auto &kv) {
                   return std::make_tuple(kv.first, kv.second.first,
                                          kv.second.second);
                 });
  return states;
}

void AMD::PMFVState::gpuState(unsigned int index,
                              units::frequency::megahertz_t freq,
                              units::voltage::millivolt_t volt)
{
  auto [gpuMin, gpuMax] = gpuRange();
  auto [voltMin, voltMax] = voltRange();
  auto &[sFreq, sVolt] = gpuStates_.at(index);
  sFreq = std::clamp(freq, gpuMin, gpuMax);
  sVolt = std::clamp(volt, voltMin, voltMax);
}

void AMD::PMFVState::memState(unsigned int index,
                              units::frequency::megahertz_t freq,
                              units::voltage::millivolt_t volt)
{
  auto [memMin, memMax] = memRange();
  auto [voltMin, voltMax] = voltRange();
  auto &[sFreq, sVolt] = memStates_.at(index);
  sFreq = std::clamp(freq, memMin, memMax);
  sVolt = std::clamp(volt, voltMin, voltMax);
}

std::string
AMD::PMFVState::ppOdClkVoltStateCmd(std::string &&type, unsigned int index,
                                    units::frequency::megahertz_t freq,
                                    units::voltage::millivolt_t volt) const
{
  return type.append(" ")
      .append(std::to_string(index))
      .append(" ")
      .append(std::to_string(freq.to<int>()))
      .append(" ")
      .append(std::to_string(volt.to<int>()));
}
