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
#pragma once

#include "core/profilepart.h"
#include "pmfvstate.h"
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace AMD {

class PMFVStateProfilePart final
: public ProfilePart
, public PMFVState::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::string const &providePMFVStateGPUVoltMode() const = 0;
    virtual std::string const &providePMFVStateMemVoltMode() const = 0;

    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMFVStateGPUState(unsigned int index) const = 0;
    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMFVStateMemState(unsigned int index) const = 0;

    virtual std::vector<unsigned int> providePMFVStateGPUActiveStates() const = 0;
    virtual std::vector<unsigned int> providePMFVStateMemActiveStates() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMFVStateGPUVoltMode(std::string const &mode) = 0;
    virtual void takePMFVStateMemVoltMode(std::string const &mode) = 0;

    virtual void takePMFVStateGPUStates(
        std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                               units::voltage::millivolt_t>> const &states) = 0;
    virtual void takePMFVStateMemStates(
        std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                               units::voltage::millivolt_t>> const &states) = 0;

    virtual void
    takePMFVStateGPUActiveStates(std::vector<unsigned int> const &indices) = 0;
    virtual void
    takePMFVStateMemActiveStates(std::vector<unsigned int> const &indices) = 0;
  };

  PMFVStateProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;

  std::string const &providePMFVStateGPUVoltMode() const override;
  std::string const &providePMFVStateMemVoltMode() const override;

  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVStateGPUState(unsigned int index) const override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVStateMemState(unsigned int index) const override;

  std::vector<unsigned int> providePMFVStateGPUActiveStates() const override;
  std::vector<unsigned int> providePMFVStateMemActiveStates() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void gpuVoltMode(std::string const &mode);
  void memVoltMode(std::string const &mode);
  void voltMode(std::string &targetMode, std::string const &mode) const;

  void gpuState(unsigned int index,
                std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t> const &value);
  void memState(unsigned int index,
                std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t> const &value);
  void
  state(unsigned int index,
        std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t> const
            &value,
        std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                               units::voltage::millivolt_t>> &targetStates,
        std::pair<units::frequency::megahertz_t,
                  units::frequency::megahertz_t> const &targetFreqRange) const;

  void gpuActivateStates(std::vector<unsigned int> const &states);
  void memActivateStates(std::vector<unsigned int> const &states);
  void activateStates(
      std::vector<unsigned int> &targetStates,
      std::vector<unsigned int> const &newActiveStates,
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &availableStates)
      const;

  class Initializer;

  std::string const id_;

  std::string gpuVoltMode_;
  std::string memVoltMode_;
  std::vector<std::string> voltModes_;

  std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> voltRange_;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> gpuFreqRange_;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> memFreqRange_;

  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      gpuStates_;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      memStates_;

  std::vector<unsigned int> gpuActiveStates_;
  std::vector<unsigned int> memActiveStates_;

  static bool const registered_;
};
} // namespace AMD
