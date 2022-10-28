// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "pmpowerstate.h"
#include <string>
#include <vector>

namespace AMD {

class PMPowerStateProfilePart final
: public ProfilePart
, public AMD::PMPowerState::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::string const &providePMPowerStateMode() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMPowerStateMode(std::string const &mode) = 0;
  };

  PMPowerStateProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const final override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;
  std::string const &providePMPowerStateMode() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void mode(std::string const &mode);

  class Initializer;

  std::string const id_;
  std::string mode_;
  std::vector<std::string> modes_;

  static bool const registered_;
};

} // namespace AMD
