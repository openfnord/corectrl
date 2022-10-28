// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "pmvoltoffset.h"
#include <string>
#include <utility>

namespace AMD {

class PMVoltOffsetProfilePart final
: public ProfilePart
, public AMD::PMVoltOffset::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual units::voltage::millivolt_t providePMVoltOffsetValue() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMVoltOffsetValue(units::voltage::millivolt_t value) = 0;
  };

  PMVoltOffsetProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;
  units::voltage::millivolt_t providePMVoltOffsetValue() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void value(units::voltage::millivolt_t value);

  class Initializer;

  std::string const id_;
  units::voltage::millivolt_t value_;
  std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> range_;

  static bool const registered_;
};

} // namespace AMD
