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

  void takePMFVStateVoltModes(std::vector<std::string> const &) override
  {
  }

  void takePMFVStateVoltRange(units::voltage::millivolt_t,
                              units::voltage::millivolt_t) override
  {
  }

  void takePMFVStateGPURange(units::frequency::megahertz_t,
                             units::frequency::megahertz_t) override
  {
  }

  void takePMFVStateMemRange(units::frequency::megahertz_t,
                             units::frequency::megahertz_t) override
  {
  }

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

  gpuVoltMode_ = pmFVStateImporter.providePMFVStateGPUVoltMode();
  memVoltMode_ = pmFVStateImporter.providePMFVStateMemVoltMode();

  for (auto &[index, stateFreq, stateVolt] : gpuStates_) {
    auto [freq, volt] = pmFVStateImporter.providePMFVStateGPUState(index);
    stateFreq = freq;
    stateVolt = volt;
  }

  for (auto &[index, stateFreq, stateVolt] : memStates_) {
    auto [freq, volt] = pmFVStateImporter.providePMFVStateMemState(index);
    stateFreq = freq;
    stateVolt = volt;
  }

  gpuActiveStates_ = pmFVStateImporter.providePMFVStateGPUActiveStates();
  memActiveStates_ = pmFVStateImporter.providePMFVStateMemActiveStates();
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
  clone->gpuVoltMode_ = gpuVoltMode_;
  clone->memVoltMode_ = memVoltMode_;
  clone->gpuStates_ = gpuStates_;
  clone->memStates_ = memStates_;
  clone->gpuActiveStates_ = gpuActiveStates_;
  clone->memActiveStates_ = memActiveStates_;

  return std::move(clone);
}

bool const AMD::PMFVStateProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFVState::ItemID, []() {
      return std::make_unique<AMD::PMFVStateProfilePart>();
    });
