// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "controlgroup.h"
#include "core/profilepart.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class ControlGroupProfilePart
: public ProfilePart
, public ControlGroup::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
  };

  class Exporter : public IProfilePart::Exporter
  {
  };

  ControlGroupProfilePart(std::string_view id) noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) final override;
  std::unique_ptr<Exportable::Exporter> initializer() final override;

  std::string const &ID() const final override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) final override;

  bool provideActive() const final override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) final override;
  void exportProfilePart(IProfilePart::Exporter &e) const final override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const final override;

  virtual std::unique_ptr<ControlGroupProfilePart> instance() const = 0;

 private:
  class Factory;
  class Initializer;

  std::vector<std::unique_ptr<IProfilePart>> parts_;
  std::string const id_;
};
