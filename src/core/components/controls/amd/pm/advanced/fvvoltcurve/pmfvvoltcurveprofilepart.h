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
#include "pmfvvoltcurve.h"
#include <string>
#include <utility>
#include <vector>

namespace AMD {

class PMFVVoltCurveProfilePart final
: public ProfilePart
, public PMFVVoltCurve::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::string const &providePMFVVoltCurveVoltMode() const = 0;

    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMFVVoltCurveVoltCurvePoint(unsigned int index) const = 0;

    virtual units::frequency::megahertz_t
    providePMFVVoltCurveGPUState(unsigned int index) const = 0;
    virtual units::frequency::megahertz_t
    providePMFVVoltCurveMemState(unsigned int index) const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMFVVoltCurveVoltMode(std::string const &mode) = 0;

    virtual void takePMFVVoltCurveVoltCurve(
        std::vector<std::pair<units::frequency::megahertz_t,
                              units::voltage::millivolt_t>> const &curve) = 0;

    virtual void takePMFVVoltCurveGPUStates(
        std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
            &states) = 0;
    virtual void takePMFVVoltCurveMemStates(
        std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
            &states) = 0;
  };

  PMFVVoltCurveProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;

  std::string const &providePMFVVoltCurveVoltMode() const override;

  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVVoltCurveVoltCurvePoint(unsigned int index) const override;

  units::frequency::megahertz_t
  providePMFVVoltCurveGPUState(unsigned int index) const override;
  units::frequency::megahertz_t
  providePMFVVoltCurveMemState(unsigned int index) const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void voltMode(std::string const &mode);

  void voltCurvePoint(unsigned int pointIndex,
                      units::frequency::megahertz_t freq,
                      units::voltage::millivolt_t volt);

  void gpuState(unsigned int index, units::frequency::megahertz_t freq);
  void memState(unsigned int index, units::frequency::megahertz_t freq);
  void
  freqState(unsigned int index, units::frequency::megahertz_t freq,
            std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>
                &targetStates,
            std::pair<units::frequency::megahertz_t,
                      units::frequency::megahertz_t> const &targetRange) const;

  class Initializer;

  std::string const id_;

  std::string voltMode_;
  std::vector<std::string> voltModes_;

  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      voltCurve_;
  std::vector<std::pair<
      std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
      std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>>
      voltCurveRange_;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> gpuStates_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> memStates_;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> gpuFreqRange_;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> memFreqRange_;

  static bool const registered_;
};
} // namespace AMD
