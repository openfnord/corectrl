// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "igpu.h"
#include <memory>
#include <string>
#include <vector>

class IControl;
class ISensor;

class GPU final : public IGPU
{
 public:
  GPU(std::unique_ptr<IGPUInfo> &&info,
      std::vector<std::unique_ptr<IControl>> &&controls,
      std::vector<std::unique_ptr<ISensor>> &&sensors)
  noexcept;

  bool active() const override;
  void activate(bool active) override;

  std::string const &key() const override;

  std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
  componentInfo() const override;

  void init() override;
  void preInit(ICommandQueue &ctlCmds) override;
  void postInit(ICommandQueue &ctlCmds) override;
  void sync(ICommandQueue &ctlCmds) override;

  void updateSensors(
      std::unordered_map<std::string, std::unordered_set<std::string>> const
          &ignored) override;

  IGPUInfo const &info() const override;

  std::string const &ID() const override;
  void importWith(Importable::Importer &i) override;
  void exportWith(Exportable::Exporter &e) const override;

 private:
  std::string const id_;
  std::unique_ptr<IGPUInfo> const info_;
  std::vector<std::unique_ptr<IControl>> const controls_;
  std::vector<std::unique_ptr<ISensor>> const sensors_;

  std::string key_;
  bool active_{true};
};
