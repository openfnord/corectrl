//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
#include "pmvoltcurve.h"
#include <string>
#include <utility>
#include <vector>

namespace AMD {

class PMVoltCurveProfilePart final
: public ProfilePart
, public PMVoltCurve::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::string const &providePMVoltCurveMode() const = 0;
    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMVoltCurvePoint(unsigned int index) const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMVoltCurveMode(std::string const &mode) = 0;
    virtual void takePMVoltCurvePoints(
        std::vector<std::pair<units::frequency::megahertz_t,
                              units::voltage::millivolt_t>> const &curve) = 0;
  };

  PMVoltCurveProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;

  std::string const &providePMVoltCurveMode() const override;

  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMVoltCurvePoint(unsigned int index) const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void mode(std::string const &mode);
  void point(unsigned int index, units::frequency::megahertz_t freq,
             units::voltage::millivolt_t volt);

  class Initializer;

  std::string const id_;

  std::string mode_;
  std::vector<std::string> modes_;

  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      points_;
  std::vector<std::pair<
      std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
      std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>>
      pointsRange_;

  static bool const registered_;
};
} // namespace AMD
