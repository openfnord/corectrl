// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "controlmode.h"
#include "core/profilepart.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class ControlModeProfilePart
: public ProfilePart
, public ControlMode::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::string const &provideMode() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takeMode(std::string const &mode) = 0;
  };

  ControlModeProfilePart(std::string_view id) noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) final override;
  std::unique_ptr<Exportable::Exporter> initializer() final override;

  std::string const &ID() const final override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) final override;

  bool provideActive() const final override;
  std::string const &provideMode() const final override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) final override;
  void exportProfilePart(IProfilePart::Exporter &e) const final override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const final override;

  virtual std::unique_ptr<ControlModeProfilePart> instance() const = 0;

 private:
  void mode(std::string const &mode);

  class Factory;
  class Initializer;

  std::vector<std::unique_ptr<IProfilePart>> parts_;
  std::string const id_;
  std::string mode_;
};
