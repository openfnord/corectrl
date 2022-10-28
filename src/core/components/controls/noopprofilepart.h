// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "noop.h"
#include <string>

class NoopProfilePart final
: public ProfilePart
, public Noop::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
  };

  class Exporter : public IProfilePart::Exporter
  {
  };

  NoopProfilePart() noexcept;

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
