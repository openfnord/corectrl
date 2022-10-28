// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "icpu.h"
#include "icpuprofilepart.h"
#include <memory>
#include <string>
#include <vector>

class CPUProfilePart final
: public ICPUProfilePart
, public ICPU::Importer
{
 public:
  CPUProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  bool belongsTo(Item const &i) const override;
  std::string const &key() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void updateKey();

  class Factory;
  class Initializer;

  std::vector<std::unique_ptr<IProfilePart>> parts_;
  std::string const id_;

  int socketId_;
  std::string key_;

  static bool const registered_;
};
