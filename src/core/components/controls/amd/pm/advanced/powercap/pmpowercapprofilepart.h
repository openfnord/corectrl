// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
