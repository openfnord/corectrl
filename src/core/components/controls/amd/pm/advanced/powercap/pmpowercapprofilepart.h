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
#include "pmpowercap.h"
#include <string>
#include <utility>

namespace AMD {

class PMPowerCapProfilePart final
: public ProfilePart
, public AMD::PMPowerCap::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual units::power::watt_t providePMPowerCapValue() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMPowerCapValue(units::power::watt_t value) = 0;
  };

  PMPowerCapProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;
  units::power::watt_t providePMPowerCapValue() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void value(units::power::watt_t value);

  class Initializer;

  std::string const id_;
  units::power::watt_t value_;
  std::pair<units::power::watt_t, units::power::watt_t> range_;

  static bool const registered_;
};

} // namespace AMD
