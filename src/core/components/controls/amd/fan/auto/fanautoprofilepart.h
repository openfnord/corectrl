// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "fanauto.h"
#include <string>

namespace AMD {

class FanAutoProfilePart final
: public ProfilePart
, public AMD::FanAuto::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
  };

  class Exporter : public IProfilePart::Exporter
  {
  };

  FanAutoProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  class Initializer;

  std::string const id_;

  static bool const registered_;
};

} // namespace AMD
