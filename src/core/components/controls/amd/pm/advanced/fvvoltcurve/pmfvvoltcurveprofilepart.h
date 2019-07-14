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

    virtual std::vector<unsigned int>
    providePMFVVoltCurveGPUActiveStates() const = 0;
    virtual std::vector<unsigned int>
    providePMFVVoltCurveMemActiveStates() const = 0;
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

    virtual void takePMFVVoltCurveGPUActiveStates(
        std::vector<unsigned int> const &indices) = 0;
    virtual void takePMFVVoltCurveMemActiveStates(
        std::vector<unsigned int> const &indices) = 0;
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

  std::vector<unsigned int> providePMFVVoltCurveGPUActiveStates() const override;
  std::vector<unsigned int> providePMFVVoltCurveMemActiveStates() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  class Initializer;

  std::string const id_;

  std::string voltMode_;

  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      voltCurve_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> gpuStates_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> memStates_;

  std::vector<unsigned int> gpuActiveStates_;
  std::vector<unsigned int> memActiveStates_;

  static bool const registered_;
};
} // namespace AMD
