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
#include "pmfvstateprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <iterator>
#include <memory>

class AMD::PMFVStateProfilePart::Initializer final
: public AMD::PMFVState::Exporter
{
 public:
  Initializer(AMD::PMFVStateProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takePMFVStateVoltModes(std::vector<std::string> const &modes) override;
  void takePMFVStateVoltRange(units::voltage::millivolt_t min,
                              units::voltage::millivolt_t max) override;
  void takePMFVStateGPURange(units::frequency::megahertz_t min,
                             units::frequency::megahertz_t max) override;
  void takePMFVStateMemRange(units::frequency::megahertz_t min,
                             units::frequency::megahertz_t max) override;

  void takeActive(bool active) override;
  void takePMFVStateGPUVoltMode(std::string const &mode) override;
  void takePMFVStateMemVoltMode(std::string const &mode) override;
  void takePMFVStateGPUStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  void takePMFVStateMemStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  void
  takePMFVStateGPUActiveStates(std::vector<unsigned int> const &indices) override;
  void
  takePMFVStateMemActiveStates(std::vector<unsigned int> const &indices) override;

 private:
  AMD::PMFVStateProfilePart &outer_;
};

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateVoltModes(
    std::vector<std::string> const &modes)
{
  outer_.voltModes_ = modes;
}

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateVoltRange(
    units::voltage::millivolt_t min, units::voltage::millivolt_t max)
{
  outer_.voltRange_ = std::make_pair(min, max);
}

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateGPURange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.gpuFreqRange_ = std::make_pair(min, max);
}

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateMemRange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.memFreqRange_ = std::make_pair(min, max);
}

void AMD::PMFVStateProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateGPUVoltMode(
    std::string const &mode)
{
  outer_.gpuVoltMode_ = mode;
}

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateMemVoltMode(
    std::string const &mode)
{
  outer_.memVoltMode_ = mode;
}

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateGPUStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  outer_.gpuStates_ = states;
}

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateMemStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  outer_.memStates_ = states;
}

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateGPUActiveStates(
    std::vector<unsigned int> const &indices)
{

  outer_.gpuActiveStates_ = indices;
}

void AMD::PMFVStateProfilePart::Initializer::takePMFVStateMemActiveStates(
    std::vector<unsigned int> const &indices)
{

  outer_.memActiveStates_ = indices;
}

AMD::PMFVStateProfilePart::PMFVStateProfilePart() noexcept
: id_(AMD::PMFVState::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFVStateProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFVStateProfilePart::initializer()
{
  return std::make_unique<AMD::PMFVStateProfilePart::Initializer>(*this);
}

std::string const &AMD::PMFVStateProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFVStateProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMFVStateProfilePart::provideActive() const
{
  return active();
}

std::string const &AMD::PMFVStateProfilePart::providePMFVStateGPUVoltMode() const
{
  return gpuVoltMode_;
}

std::string const &AMD::PMFVStateProfilePart::providePMFVStateMemVoltMode() const
{
  return memVoltMode_;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFVStateProfilePart::providePMFVStateGPUState(unsigned int index) const
{
  auto stateIt = std::find_if(
      gpuStates_.cbegin(), gpuStates_.cend(),
      [=](auto &state) { return std::get<0>(state) == index; });

  if (stateIt != gpuStates_.cend())
    return std::make_pair(std::get<1>(*stateIt), std::get<2>(*stateIt));
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFVStateProfilePart::providePMFVStateMemState(unsigned int index) const
{
  auto stateIt = std::find_if(
      memStates_.cbegin(), memStates_.cend(),
      [=](auto &state) { return std::get<0>(state) == index; });

  if (stateIt != memStates_.cend())
    return std::make_pair(std::get<1>(*stateIt), std::get<2>(*stateIt));
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

std::vector<unsigned int>
AMD::PMFVStateProfilePart::providePMFVStateGPUActiveStates() const
{
  return gpuActiveStates_;
}

std::vector<unsigned int>
AMD::PMFVStateProfilePart::providePMFVStateMemActiveStates() const
{
  return memActiveStates_;
}

void AMD::PMFVStateProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmFVStateImporter =
      dynamic_cast<AMD::PMFVStateProfilePart::Importer &>(i);

  gpuVoltMode(pmFVStateImporter.providePMFVStateGPUVoltMode());
  memVoltMode(pmFVStateImporter.providePMFVStateMemVoltMode());

  for (auto &[index, _1, _2] : gpuStates_)
    gpuState(index, pmFVStateImporter.providePMFVStateGPUState(index));

  for (auto &[index, _1, _2] : memStates_)
    memState(index, pmFVStateImporter.providePMFVStateMemState(index));

  gpuActivateStates(pmFVStateImporter.providePMFVStateGPUActiveStates());
  memActivateStates(pmFVStateImporter.providePMFVStateMemActiveStates());
}

void AMD::PMFVStateProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmFVStateExporter =
      dynamic_cast<AMD::PMFVStateProfilePart::Exporter &>(e);

  pmFVStateExporter.takePMFVStateGPUVoltMode(gpuVoltMode_);
  pmFVStateExporter.takePMFVStateMemVoltMode(memVoltMode_);
  pmFVStateExporter.takePMFVStateGPUStates(gpuStates_);
  pmFVStateExporter.takePMFVStateMemStates(memStates_);
  pmFVStateExporter.takePMFVStateGPUActiveStates(gpuActiveStates_);
  pmFVStateExporter.takePMFVStateMemActiveStates(memActiveStates_);
}

std::unique_ptr<IProfilePart> AMD::PMFVStateProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMFVStateProfilePart>();

  clone->voltModes_ = voltModes_;
  clone->gpuVoltMode_ = gpuVoltMode_;
  clone->memVoltMode_ = memVoltMode_;

  clone->voltRange_ = voltRange_;
  clone->gpuFreqRange_ = gpuFreqRange_;
  clone->memFreqRange_ = memFreqRange_;

  clone->gpuStates_ = gpuStates_;
  clone->memStates_ = memStates_;

  clone->gpuActiveStates_ = gpuActiveStates_;
  clone->memActiveStates_ = memActiveStates_;

  return std::move(clone);
}

void AMD::PMFVStateProfilePart::gpuVoltMode(std::string const &mode)
{
  voltMode(gpuVoltMode_, mode);
}

void AMD::PMFVStateProfilePart::memVoltMode(std::string const &mode)
{
  voltMode(memVoltMode_, mode);
}

void AMD::PMFVStateProfilePart::voltMode(std::string &targetMode,
                                         std::string const &mode) const
{
  auto iter = std::find_if(
      voltModes_.cbegin(), voltModes_.cend(),
      [&](auto &availableMode) { return mode == availableMode; });
  if (iter != voltModes_.cend())
    targetMode = mode;
}

void AMD::PMFVStateProfilePart::gpuState(
    unsigned int index,
    std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t> const &value)
{
  state(index, value, gpuStates_, gpuFreqRange_);
}

void AMD::PMFVStateProfilePart::memState(
    unsigned int index,
    std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t> const &value)
{
  state(index, value, memStates_, memFreqRange_);
}

void AMD::PMFVStateProfilePart::state(
    unsigned int index,
    std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t> const &value,
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> &targetStates,
    std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const
        &targetFreqRange) const
{
  auto stateIt = std::find_if(
      targetStates.begin(), targetStates.end(),
      [=](auto &state) { return std::get<0>(state) == index; });

  if (stateIt != targetStates.end()) {
    auto &[_, sFreq, sVolt] = *stateIt;
    sFreq = std::clamp(value.first, targetFreqRange.first,
                       targetFreqRange.second);
    sVolt = std::clamp(value.second, voltRange_.first, voltRange_.second);
  }
}

void AMD::PMFVStateProfilePart::gpuActivateStates(
    std::vector<unsigned int> const &states)
{
  activateStates(gpuActiveStates_, states, gpuStates_);
}

void AMD::PMFVStateProfilePart::memActivateStates(
    std::vector<unsigned int> const &states)
{
  activateStates(memActiveStates_, states, memStates_);
}

void AMD::PMFVStateProfilePart::activateStates(
    std::vector<unsigned int> &targetStates,
    std::vector<unsigned int> const &newActiveStates,
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &availableStates) const
{
  std::vector<unsigned int> active;
  std::copy_if(newActiveStates.cbegin(), newActiveStates.cend(),
               std::back_inserter(active), [&](unsigned int index) {
                 // skip unknown state indices
                 return std::find_if(availableStates.cbegin(),
                                     availableStates.cend(), [&](auto &state) {
                                       return std::get<0>(state) == index;
                                     }) != availableStates.cend();
               });
  if (!active.empty()) // at least one state must be active
    std::swap(active, targetStates);
}

bool const AMD::PMFVStateProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFVState::ItemID, []() {
      return std::make_unique<AMD::PMFVStateProfilePart>();
    });
