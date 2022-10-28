// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "fanfixed.h"
#include <string>

namespace AMD {

class FanFixedProfilePart final
: public ProfilePart
, public AMD::FanFixed::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual units::concentration::percent_t provideFanFixedValue() const = 0;
    virtual bool provideFanFixedFanStop() const = 0;
    virtual units::concentration::percent_t
    provideFanFixedFanStartValue() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takeFanFixedValue(units::concentration::percent_t value) = 0;
    virtual void takeFanFixedFanStop(bool enabled) = 0;
    virtual void
    takeFanFixedFanStartValue(units::concentration::percent_t value) = 0;
  };

  FanFixedProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;
  units::concentration::percent_t provideFanFixedValue() const override;
  bool provideFanFixedFanStop() const override;
  units::concentration::percent_t provideFanFixedFanStartValue() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void value(units::concentration::percent_t value);
  void startValue(units::concentration::percent_t value);

  class Initializer;

  std::string const id_;

  units::concentration::percent_t value_;

  bool fanStop_;
  units::concentration::percent_t fanStartValue_;

  static bool const registered_;
};

} // namespace AMD
