// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "igpu.h"
#include "igpuprofilepart.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class GPUProfilePart final
: public IGPUProfilePart
, public IGPU::Importer
{
 public:
  GPUProfilePart() noexcept;

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
  std::string deviceID_;
  std::string revision_;
  std::string key_;
  int index_{0};
  std::optional<std::string> uniqueID_;

  static bool const registered_;
};
